#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <time.h>
#include <esp_sntp.h>
#include <esp_timer.h>

#include "config.h"
#include "CatProfile.h"

/* Globals */
int LED = 13;

uint32_t catProfileVersion = 0;           // ID for this cat profile; shows when update is needed.
uint32_t numCats = 0;                     // Number of profiles active on this device.
catProfile profileBuffer[NUM_CATS] = {0}; // Working copy of cat profiles.

volatile int updateFlag = 0;                    // Bool: pending catProfile update is available?
catProfileServer updateBuffer[NUM_CATS] = {0};  // Stores pending updates to cat profiles.
SemaphoreHandle_t updateLock = xSemaphoreCreateMutex();

void ISR1(void* arg) {
  debugPrint("Timer 1 End", esp_timer_get_time() / 1000);
}

void ISR2(void* arg) {
  debugPrint("Timer 2 End", esp_timer_get_time() / 1000);
}

void ISR3(void* arg) {
  debugPrint("Timer 3 End", esp_timer_get_time() / 1000);
}

void interruptTest() {
  esp_timer_handle_t h1;
  esp_timer_handle_t h2;
  esp_timer_handle_t h3;

  const esp_timer_create_args_t arg1 = {
    (esp_timer_cb_t)&ISR1, 
    (void*)NULL, 
    (esp_timer_dispatch_t)0, 
    "Timer 1"
  };

  const esp_timer_create_args_t arg2 = {
    (esp_timer_cb_t)&ISR2, 
    (void*)NULL, 
    (esp_timer_dispatch_t)0, 
    "Timer 2"
  };

  const esp_timer_create_args_t arg3 = {
    (esp_timer_cb_t)&ISR3, 
    (void*)NULL, 
    (esp_timer_dispatch_t)0, 
    "Timer 3"
  };

  esp_timer_create(&arg1, &h1);
  esp_timer_create(&arg2, &h2);
  esp_timer_create(&arg3, &h3);

  esp_timer_start_once(h1, 5000000);
  esp_timer_start_once(h2, 5000000);
  esp_timer_start_once(h3, 0);
  esp_timer_stop(h3);
  debugPrint("Timers Start", esp_timer_get_time() / 1000);
}

void setup() {
#if DEBUG_MODE
  // Enable serial monitor
  Serial.begin(115200);

  // Initialize pin 13 for debug purposes
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
#endif

  // Initialize System
//  initWiFi();
//  initTime();
//  initCatProfiles();

  // Interrupt test
  interruptTest();

  // Time test
}

void loop() {

}
