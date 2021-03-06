// Reconnect to wifi. Automatically called by library upon disconnect event.
void wifiReconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  debugPrint("WiFi Connection Lost; Reconnecting...", NULL);
  WiFi.reconnect();
}

// Connect to WiFi in station mode using (SSID, PWD)
void initWiFi() {
//  verbosePrint("MAC Address", WiFi.macAddress());
//  verbosePrint("Number of networks found", WiFi.scanNetworks());

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
#if EDU_WIFI
  WiFi.begin(SSID);
#else
  WiFi.begin(SSID, PWD);
#endif

  debugPrint("Connecting to WiFi", NULL);

  while (WiFi.status() != WL_CONNECTED);

  debugPrint("Wifi Connected", NULL);

  // Register handler to reconnect to WiFi if disconnected.
  WiFi.onEvent(wifiReconnect, SYSTEM_EVENT_STA_DISCONNECTED);
}

// Contact SNTP server and sync system time; set timezone to CST.
void initSNTP() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();

  // Time zone CST/CDT. American DST.
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
  // Ret val of -1 means the server failed to respond correctly or on time. 
  // Block until good resp received.
  while (retrieveCatProfiles() == -1)
    delay(5000);

  updateCatProfiles();

  debugPrint("Profiles Received and Initialized", NULL);
}

// Initialize UART driver and install.
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html
void initUARTDriver() {
  // Set driver config
  uart_config_t uartConfig = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  // Apply config
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uartConfig));

  // Set COMM pins
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2,
                               TX_PIN,
                               RX_PIN, 
                               UART_PIN_NO_CHANGE,
                               UART_PIN_NO_CHANGE));

  // Install driver
  uart_driver_install(UART_NUM_2, UART_FIFO_LEN * 2, 0, 20, &uartQueue, 0); 

  // Launch handler task
  xTaskCreate(uartHandler, "UART Handler", 2048, NULL, 12, NULL);
}

// Initialize all the microcontroller pins needed to communicate with the RFID, Load Cell, and Motor.
void initHardwarePins() {
  pinMode(RFID_NEARBY, INPUT);    // "Tag Nearby" of RFID sensor
  pinMode(CELL_DOUT, INPUT);
  pinMode(CELL_SCLK, OUTPUT);
  pinMode(CELL_PWDN, OUTPUT);
  pinMode(CELL_SPEED, OUTPUT);

  // Example code from: https://github.com/sparkfun/HX711-Load-Cell-Amplifier/tree/V_1.1/firmware
  scale.begin(CELL_DOUT, CELL_SCLK);
  scale.set_scale(CELL_CALIB);

  digitalWrite(CELL_PWDN, HIGH);  // Keep high so as not to start powerdown mode
  digitalWrite(CELL_SPEED, LOW);  // Keep low for 10 SPS.

  scale.tare(CELL_TARES);

  pinMode(MOTOR_STBY, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(MOTOR_STBY, LOW);    // Hold motor on standby until directional pins set.
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, HIGH);
  digitalWrite(MOTOR_STBY, HIGH);
}

// Create timer used to mark 1 min periods during a portion.
void initDispTimer() {
  esp_timer_create(&dispTimerArgs, &dispTimer);
}
