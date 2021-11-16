#include <WiFi.h>
#include <time.h>
#include <esp_sntp.h>
#include <esp_timer.h>
#include <driver/uart.h>
#include <ArduinoHttpClient.h>
#include <HX711.h>

#include "config.h"
#include "CatProfile.h"

/* Globals */
esp_timer_handle_t clientTimer;                 // 1 min timer that launches client task.
esp_timer_handle_t dispTimer;                   // Timer to count 1 min period during the portion.

volatile uint8_t dispFlag = 0;                  // Indicates when 1 min period passed.

volatile uint64_t catID = 0;                    // RFID tag ID of newly arrived cat.
SemaphoreHandle_t catIDLock = xSemaphoreCreateMutex();

uint32_t catProfileVersion = 0;                 // Server versioning ID for profileBuffer.
catProfile profileBuffer[NUM_CATS] = {0};       // Working copy of cat profiles.

volatile uint8_t updateFlag = 0;                // Indicates whether catProfile update is available.
catProfileServer updateBuffer[NUM_CATS] = {0};  // Stores pending updates to cat profiles.
SemaphoreHandle_t updateLock = xSemaphoreCreateMutex();

QueueHandle_t uartQueue;                            // Queues UART events.
QueueHandle_t sendQueue;                            // Queues data packets to be sent to server.

HX711 scale;

// Sets timeEINTR flag for given profile when timer expires.
void ISR(void* arg) {
  volatile uint8_t *timeEINTR = (volatile uint8_t *)arg;

  *timeEINTR = 1;
  debugPrint("ISR Fired", NULL);
}

const esp_timer_create_args_t intrArgs[] = {
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[0].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "ISR: Profile 1 EINTR"
  },
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[1].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "ISR: Profile 2 EINTR"
  },
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[2].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "ISR: Profile 3 EINTR"
  }
};

// Set dispFlag to indicate 1 min period elapsed.
void dispISR(void* arg) {
  dispFlag = 1;
}

const esp_timer_create_args_t dispTimerArgs = {
    (esp_timer_cb_t)&dispISR, 
    (void*)NULL, 
    (esp_timer_dispatch_t)0, 
    "ISR: Set dispFlag"
};

void setup() {
#if DEBUG_MODE || VERBOSE_MODE
  // Enable serial monitor
  Serial.begin(115200);
#endif

  // Initialize System
  initWiFi();
  initSNTP();
  initTimer();
  initCatProfiles();

  // Init driver tasks
  initUARTDriver();
  initHardwarePins();
  initDispTimer();

  sendQueue = xQueueCreate(1024, sizeof(dataPacket));

  // Launch client task (communicates with remote server)
  initClientTask();

  // Device task launched through loop()
}

void loop() {
start:
  // Get most up to date information before proceeding
  checkEINTR();
  updateCatProfiles();

  // Retrieve profileBuffer index of cat at bowl.
  int catIndex = nearbyCat();
  if ((catIndex == -1) || (profileBuffer[catIndex].inUse == 0))
    goto start;

  catProfile *p = &profileBuffer[catIndex];

  // Check whether cat is allowed to eat
  if ((p->canEat == 0) || (p->isComplete == 1))
     goto start;

  // Initialize portion if this is the first time activating
  if (p->inProgress == 0) {
    p->amountEaten = 0;
    p->inProgress = 1;
    p->isComplete = 0;
  }

  // Update state when portion has been fully dispensed.
  if (p->amountEaten >= p->portionGrams) {
    p->amountEaten = 0;
    p->inProgress = 0;
    p->isComplete = 1;

    goto start;
  }

  // Arm dispense timer.
  esp_timer_start_once(dispTimer, POLL_PERIOD);

  // Drive motor until weight reaches max per period or max per portion.
  float weight = scale.get_units(CELL_READS);

  debugPrint("Max Rate", p->maxRate);
  debugPrint("Initial Weight", weight);

  if (weight < p->maxRate) {
    // Turn on motor.
    digitalWrite(MOTOR_IN1, LOW);

    while (weight < p->maxRate) {
      // Break if overall portion limit is reached
      if ((p->amountEaten + weight) >= p->portionGrams)
        break;
  
      weight = scale.get_units(CELL_READS);
      debugPrint("Current Weight", weight);
    }
  }

  // Turn off motor.
  digitalWrite(MOTOR_IN1, HIGH);

  // Wait until disp timer elapses before dispensing next portion
  while (dispFlag == 0);

  // TODO: Break and stop timer if cat leaves AND new cat arrives.

  dispFlag = 0;

  // Increment amountEaten
  float amountEaten = weight - scale.get_units(CELL_READS);
  if (amountEaten < 0)
    amountEaten = 0;

  p->amountEaten += amountEaten;

  // Send data
  dataPacket newData = {catIndex, amountEaten, (TIME_T)time(NULL)};
  xQueueSend(sendQueue, &newData, 0);

  printState(catIndex);
}
