void clientTask() {
  retrieveCatProfiles();    // Ignore ret value; we poll every minute.

  // Send data to server.
  dataPacket newData = {0};

  WiFiClient c;
  HttpClient client(c, SERVER_IP, SERVER_PORT);
  while (xQueueReceive(sendQueue, &newData, 0)) {
    // Convert host bytes to network order.
    NTOHL((uint8_t*)&newData.profileIndex);
    NTOHL((uint8_t*)&newData.amountEaten);
    NTOHLL((uint8_t*)&newData.timeStamp);
    
    client.post("BowlData", "application/bowl-data", sizeof(dataPacket), (uint8_t*)&newData);
  }
  client.stop();
}

const esp_timer_create_args_t clientTaskArgs = {
    (esp_timer_cb_t)&clientTask, 
    NULL, 
    ESP_TIMER_TASK, 
    "Task: Retrieve cat profiles/Send new bowl data"
};


void initClientTask() {
  esp_timer_create(&clientTaskArgs, &clientTimer);
  esp_timer_start_periodic(clientTimer, POLL_PERIOD);
}
