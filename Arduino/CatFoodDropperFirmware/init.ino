// Connect to WiFi in station mode using (SSID, PWD)
void initWiFi() {
  verbosePrint("MAC Address", WiFi.macAddress());
//  verbosePrint("Number of networks found", WiFi.scanNetworks());

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED) {
#if DEBUG_MODE || VERBOSE_MODE
    // Blink LED while connecting; leave on after connected
    digitalWrite(LED, LOW);
    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
#endif
  }
}

// Contact SNTP server and sync system time.
void initSNTP() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}

// Create timers that can be used for interrupts for each profile.
// See 'interrupts' file for more details.
void initTimer() {
  for (int i = 0; i < NUM_CATS; ++i)
    esp_timer_create(&intrArgs[i], &profileBuffer[i].timerHandle);
}

// Retrieve cat profile from server and update device copy.
void initCatProfiles() {
  // Initialize data semaphores.
  for (int i = 0; i < NUM_CATS; ++i)
    profileBuffer[i].dataLock = xSemaphoreCreateMutex();

  // Ret val of -1 means the server failed to respond correctly or on time. 
  // Block until good resp received.
  while (retrieveCatProfiles() == -1)
    delay(5000);

  updateCatProfiles();
}
