#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <time.h>
#include <esp_sntp.h>
#include <esp_timer.h>

#include "config.h"
#include "CatProfile.h"

/* Globals */
int LED = 13;

uint32_t catProfileVersion = 0;                 // Server versioning ID for profileBuffer.
catProfile profileBuffer[NUM_CATS] = {0};       // Working copy of cat profiles.

volatile int updateFlag = 0;                    // Indicates whether catProfile update is available.
catProfileServer updateBuffer[NUM_CATS] = {0};  // Stores pending updates to cat profiles.
SemaphoreHandle_t updateLock = xSemaphoreCreateMutex();

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

  // Launch client task (communicates with remote server.
  esp_timer_handle_t clientTaskHandle;
  launchClientTask(&clientTaskHandle);

  // Device task launched through loop().
}

void loop() {
  // Test interrupt scheduling
  if (profileBuffer[0].timeEINTR)
    updateState(&profileBuffer[0]);
  else if (profileBuffer[1].timeEINTR)
    updateState(&profileBuffer[1]);
  else if (profileBuffer[2].timeEINTR)
    updateState(&profileBuffer[2]);
}
