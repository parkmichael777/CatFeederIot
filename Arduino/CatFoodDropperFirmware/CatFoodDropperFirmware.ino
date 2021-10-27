#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <time.h>
#include <esp_sntp.h>
#include <esp_timer.h>
#include <driver/uart.h>

#include "config.h"
#include "CatProfile.h"

/* Globals */
esp_timer_handle_t clientTaskHandle;
esp_timer_handle_t dispTimerHandle;             // Timer to count 1 min period during the portion.

volatile uint8_t dispFlag = 0;                  // Indicates when 1 min period passed.

volatile uint64_t catID = 0;                    // RFID tag ID of newly arrived cat.
SemaphoreHandle_t catIDLock = xSemaphoreCreateMutex();

uint32_t catProfileVersion = 0;                 // Server versioning ID for profileBuffer.
catProfile profileBuffer[NUM_CATS] = {0};       // Working copy of cat profiles.

volatile uint8_t updateFlag = 0;                // Indicates whether catProfile update is available.
catProfileServer updateBuffer[NUM_CATS] = {0};  // Stores pending updates to cat profiles.
SemaphoreHandle_t updateLock = xSemaphoreCreateMutex();

QueueHandle_t queue;                            // Queues UART events.

// Sets timeEINTR flag for given profile when timer expires.
void ISR(void* arg) {
  volatile uint8_t *timeEINTR = (volatile uint8_t *)arg;

  debugPrint("ISR Core", xPortGetCoreID());

  *timeEINTR = 1;
  debugPrint("ISR Fired", NULL);
}

const esp_timer_create_args_t intrArgs[] = {
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[0].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "Profile 1 ISR"
  },
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[1].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "Profile 2 ISR"
  },
  {
    (esp_timer_cb_t)&ISR, 
    (void*)&profileBuffer[2].timeEINTR, 
    (esp_timer_dispatch_t)0, 
    "Profile 3 ISR"
  }
};

void setup() {
#if DEBUG_MODE || VERBOSE_MODE
  // Enable serial monitor
  Serial.begin(115200);

  // Initialize pin 13 for debug purposes
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
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

  // Launch client task (communicates with remote server)
  initClientTask();

  // Device task launched through loop()
}

void loop() {
start:
  // Get most up to date information before proceeding
  checkEINTR();
  updateCatProfiles();

  // TODO: Add delay inside 'NEARBY?' if-statement to prevent wasted cycles?

  if (digitalRead(NEARBY) == LOW)
    goto start;

  // TODO: Get lock, copy catID, and decode to corresponding profileBuffer.

//  catProfile *p = &profileBuffer[catIDtoProfile(catID)];

  // Check whether cat is allowed to eat
  if ((p->canEat == 0) || (p->isComplete == 1))
     goto start;

  // Initialize portion if this is the first time activating
  if (p->inProgress == 0) {
    // TODO: Set tare weight.

    // Clear data collected from last portion
    xSemaphoreTake(p->dataLock, portMAX_DELAY);
    memset(&p->data, 0, sizeof(bowlData));
    xSemaphoreGive(p->dataLock);

    p->inProgress = 1;
  }

  // TODO: Check if amount dispensed exceeds max per portion
  // if yes, set isComplete, inProgress, and sendData.

  // Arm dispense timer.
  esp_timer_start_once(dispTimerHandle, POLL_PERIOD);

  // TODO: Read initial weight (current weight - tare weight).

  // TODO: Drive motor until weight reaches max per period or max per portion.
  // TODO: If max per portion is met, set isComplete, inProgress, and sendData.

  // TODO: Atomically increment bowlData.

  // TODO: Wait until disp timer elapses; set dispFlag to 0.
}
