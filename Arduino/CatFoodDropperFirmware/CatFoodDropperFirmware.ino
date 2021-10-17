#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <sys/time.h>
#include <esp_sntp.h>

#include "config.h"
#include "CatProfile.h"

int LED = 13;

/* Globals */
catProfileServer updateBuffer[3] = {0}; // Stores pending updates to cat profiles.
catProfile profile[3] = {0};            // Working copy of cat profiles.

void ISR() {
  digitalWrite(LED, !digitalRead(LED));
}

void interruptTest() {
  hw_timer_t *timer = NULL;

  digitalWrite(LED, LOW);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &ISR, true);

  // Sets an alarm to sound every second
  timerAlarmWrite(timer, 1000000, true);

  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
  timerAlarmEnable(timer);
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
  initWiFi();
  initTime();

  // Setup cat profiles
//  initCatProfiles();

  // Interrupt test
//  interruptTest();

  // Time test
}

void loop() {

}
