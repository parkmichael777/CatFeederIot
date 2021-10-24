void clientTask() {
  retrieveCatProfiles();    // Ignore ret value; we poll every minute.

  // TODO: temporarily yield here if task triggers WDT.

  // TODO: Call sendData function here.
}

const esp_timer_create_args_t clientTaskArgs = {
    (esp_timer_cb_t)&clientTask, 
    NULL, 
    ESP_TIMER_TASK, 
    "Task: Retrieve cat profiles/Send new bowl data"
};


void launchClientTask(esp_timer_handle_t *clientTaskHandle) {
  esp_timer_create(&clientTaskArgs, clientTaskHandle);
  esp_timer_start_periodic(*clientTaskHandle, POLL_PERIOD);
}
