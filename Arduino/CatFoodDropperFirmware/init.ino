// Reconnect to wifi. Automatically called by library upon disconnect event.
void wifiReconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  debugPrint("WiFi Connection Lost; Reconnecting...", NULL);
  WiFi.reconnect();
}

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

  // Register handler to reconnect to WiFi if disconnected.
  WiFi.onEvent(wifiReconnect, SYSTEM_EVENT_STA_DISCONNECTED);
}

// Contact SNTP server and sync system time; set timezone to CST.
void initSNTP() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();

  setenv("TZ", "CST+6CDT,M3.2.0/2,M11.1.0/2", 1);
  tzset();
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
