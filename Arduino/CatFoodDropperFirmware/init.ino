void initWiFi() {
#if DEBUG_MODE
  //  Serial.print("Number of networks found ");
  //  Serial.println(WiFi.scanNetworks());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
#endif

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

void initTime() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}

void initCatProfiles() {
  WiFiClient c;
  HttpClient client(c, SERVER_IP, SERVER_PORT);

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
