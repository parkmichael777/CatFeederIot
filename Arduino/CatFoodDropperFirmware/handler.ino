// Used for unhandled response codes. Theoretically, should never be called.
int handleStub(int status) {
  debugPrint("Error: Unhandled Response Code", status);
  while(true);

  return -1;  // Not reachable.
}

// Handle 200 OK for a GET CatProfiles request.
int handle200OK(HttpClient & client) {
  // Retrieve response
  while (client.endOfHeadersReached() == false) {
    client.headerAvailable();
    
    String headerName = client.readHeaderName();
  
    if (headerName.equals("Cat-Profile-Version")) {
      catProfileVersion = (uint32_t)(client.readHeaderValue().toInt());
      client.skipResponseHeaders();
    }
  }
  
  xSemaphoreTake(updateLock, portMAX_DELAY);
  
  client.read((uint8_t*)updateBuffer, 3 * sizeof(catProfileServer));
  
  // Fix byte order of maxRate and portion times.
  for (int i = 0; i < NUM_CATS; ++i) {
    NetworkToHostL((uint8_t*)(&updateBuffer[i]));
  
    for (int j = 0; j < NUM_PORTIONS; ++j) {
      NetworkToHostL((uint8_t*)(&updateBuffer[i].portionTimes[j]));
    }
  }
  
  // Raise update flag
  updateFlag = 1;
  
  xSemaphoreGive(updateLock);
  
  client.stop();
  
  printUpdateBuffer();

  return 0;
}

// Debug info for 204 No Content resp to a GET CatProfiles request.
int handle204NoContent() {
  debugPrint("No Content", NULL);
  return -1;
}
