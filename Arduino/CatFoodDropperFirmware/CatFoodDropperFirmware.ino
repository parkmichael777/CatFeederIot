#include <WiFi.h>

#define DEBUG_MODE 1

const char *ssid = "NETGEAR13";
const char *pwd = "largewater374";

int LED = 13;

void initWiFi() {
  Serial.print("Number of networks found ");
  Serial.println(WiFi.scanNetworks());

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  while (true) {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(1000);
  }
}

void setup() {
  // Enable Serial monitor for debugging
#if DEBUG_MODE
  Serial.begin(115200);
#endif

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  // Setup WiFi
  initWiFi();

}

void loop() {
  
}
