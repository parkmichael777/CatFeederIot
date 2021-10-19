void initWiFi() {
  verbosePrint("MAC Address", WiFi.macAddress());
  verbosePrint("Number of networks found", WiFi.scanNetworks());

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

void printCatProfiles() {
  for (int i = 0; i < NUM_CATS; ++i) {
    verbosePrint("Cat Profile " + String(i), NULL);
    verbosePrint("Max Rate", updateBuffer[i].maxRate);
    verbosePrint("Portion Grams", updateBuffer[i].portionGrams);
    verbosePrint("Num Portions", updateBuffer[i].numPortions);

    for (int j = 0; j < NUM_PORTIONS; ++j)
      verbosePrint("P" + String(j), updateBuffer[i].portionTimes[j]);

    verbosePrint(NULL, NULL);
  }
}

void NetworkToHostL(uint8_t *bytes) {
  // ntohl/htonl function that works with floats
  uint8_t b0, b1, b2, b3;

  b0 = bytes[0];
  b1 = bytes[1];
  b2 = bytes[2];
  b3 = bytes[3];

  bytes[0] = b3;
  bytes[1] = b2;
  bytes[2] = b1;
  bytes[3] = b0;
}

void initCatProfiles() {
  WiFiClient c;
  HttpClient client(c, SERVER_IP, SERVER_PORT);

  // Request CatProfiles
  while (client.get("CatProfiles") == -1) {
    delay(5000);
  }

  int status = client.responseStatusCode();

  // Update profiles if new 
  if (status == 200) {
    
  }
  else if (status == 204) {
    pass;
  }
  else {
    debugPrint("Request failed; Response Code", status);
    while(true);
  }

  // Retrieve response
  client.skipResponseHeaders();

  xSemaphoreTake(updateLock, portMAX_DELAY);

  client.read((uint8_t*)updateBuffer, 3 * sizeof(catProfileServer));

  // Fix byte order of maxRate.
  for (int i = 0; i < NUM_CATS; ++i)
    NetworkToHostL((uint8_t*)(&updateBuffer[i]));

  // Raise update flag
  update = 1;

  xSemaphoreGive(updateLock);

  printCatProfiles();
}
