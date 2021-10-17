#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <sys/time.h>
#include "esp_sntp.h"

#include <CatFoodDropper/CatProfile.h>

#define DEBUG_MODE 1

int LED = 13;

const char *ssid = "NETGEAR13";
const char *pwd = "largewater374";

const char *ServerIP = "192.168.1.3";
const uint16_t ServerPort = 8000;

CatProfileServer updateBuffer[3] = {0};   // Stores pending updates to cat profiles.
CatProfile catProfile[3] = {0};           // Working copy of cat profiles.

void initWiFi() {
#if DEBUG_MODE
  //  Serial.print("Number of networks found ");
  //  Serial.println(WiFi.scanNetworks());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
#endif

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
#if DEBUG_MODE
    // Blink LED while connecting; leave on after connected
    digitalWrite(LED, LOW);
    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
#endif
  }
}

void initCatProfiles() {
  WiFiClient c;
  HttpClient client(c, ServerIP, ServerPort);

  do {
    client.get("catProfiles.json");
  }
  while (client.responseStatusCode() != 200);

  client.skipResponseHeaders();

  catProfileServer updateBuffer[3] = {0};
  client.read((uint8_t*)updateBuffer, 3 * sizeof(catProfileServer));

  Serial.print("Read data: ");
  Serial.println(updateBuffer[0].maxRate);
}

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

void initTime() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
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
