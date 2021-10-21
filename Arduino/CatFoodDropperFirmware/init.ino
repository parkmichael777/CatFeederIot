// Connect to WiFi in station mode using (SSID, PWD)
void initWiFi() {
  verbosePrint("MAC Address", WiFi.macAddress());
//  verbosePrint("Number of networks found", WiFi.scanNetworks());

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);

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

// Contact SNTP server and sync system time.
void initTime() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}

// Retrieve cat profile from server and update device copy.
void initCatProfiles() {
  // -1 Means the GET Request failed, or the server hasn't been initialized with
  // a valid Cat Profile yet. Wait until server is ready.
  while (retrieveCatProfiles() == -1)
    delay(5000);

  updateCatProfiles();
}
