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

  *timeEINTR = 1;
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

  // Interrupt test
//  interruptTest();

  // Time test
}

void loop() {

}
