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

#if DEBUG_MODE
void printCatProfiles() {
  Serial.println("Cat Profile 0: ");
  Serial.println(updateBuffer[0].maxRate);
  Serial.println(updateBuffer[0].portionGrams);
  Serial.println(updateBuffer[0].numPortions);
  Serial.println(updateBuffer[0].portionTimes[0]);
  Serial.println(updateBuffer[0].portionTimes[1]);
  Serial.println(updateBuffer[0].portionTimes[2]);
  Serial.println(updateBuffer[0].portionTimes[3]);
  Serial.println(updateBuffer[0].portionTimes[4]);
  Serial.println();

  Serial.println("Cat Profile 1: ");
  Serial.println(updateBuffer[1].maxRate);
  Serial.println(updateBuffer[1].portionGrams);
  Serial.println(updateBuffer[1].numPortions);
  Serial.println(updateBuffer[1].portionTimes[0]);
  Serial.println(updateBuffer[1].portionTimes[1]);
  Serial.println(updateBuffer[1].portionTimes[2]);
  Serial.println(updateBuffer[1].portionTimes[3]);
  Serial.println(updateBuffer[1].portionTimes[4]);
  Serial.println();
  
  Serial.println("Cat Profile 2: ");
  Serial.println(updateBuffer[2].maxRate);
  Serial.println(updateBuffer[2].portionGrams);
  Serial.println(updateBuffer[2].numPortions);
  Serial.println(updateBuffer[2].portionTimes[0]);
  Serial.println(updateBuffer[2].portionTimes[1]);
  Serial.println(updateBuffer[2].portionTimes[2]);
  Serial.println(updateBuffer[2].portionTimes[3]);
  Serial.println(updateBuffer[2].portionTimes[4]);
  Serial.println();
}
#endif

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

  do {
    client.get("CatProfiles");
  }
  while (client.responseStatusCode() != 200);

  client.skipResponseHeaders();
  Serial.print("Bytes Read: ");
  Serial.println(client.read((uint8_t*)updateBuffer, 3 * sizeof(catProfileServer)));

  for (int i = 0; i < NUM_CATS; ++i) {
    NetworkToHostL((uint8_t*)(&updateBuffer[i]));
  }

#if DEBUG_MODE
  printCatProfiles();
#endif
}
