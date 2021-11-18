// Used for unhandled response codes. Theoretically, should never be called.
int handleStub(int status) {
  debugPrint("Error: Unhandled Response Code", status);
  while(true);

  return -1;  // Not reachable.
}

// Handle 200 OK for a GET CatProfiles request.
int handle200OK(HttpClient & client) {
  int versionFlag = 0;

  // Retrieve response
  while (client.endOfHeadersReached() == false) {
    client.headerAvailable();

    String headerName = client.readHeaderName();

    if (headerName.equals("Cat-Profile-Version")) {
      catProfileVersion = client.readHeaderValue().toInt();
      versionFlag = 1;

      client.skipResponseHeaders();
    }
  }

  // Malformed Response
  if (versionFlag == 0)
    return -1;

  // Process Response
  xSemaphoreTake(updateLock, portMAX_DELAY);
  
  client.read((uint8_t*)updateBuffer, 3 * sizeof(catProfileServer));
  
  // Fix byte order of maxRate and portion times.
  for (int i = 0; i < NUM_CATS; ++i) {
    if (updateBuffer[i].inUse == 0)
      continue;

    // Fix maxRate (4 bytes)
    NTOHL((uint8_t*)(&updateBuffer[i].maxRate));

    // Fix portionGrams (4 bytes)
    NTOHL((uint8_t*)(&updateBuffer[i].portionGrams));

    // Fix portionTimes (8 bytes each)
    for (int j = 0; j < updateBuffer[i].numPortions; ++j)
      NTOHLL((uint8_t*)(&updateBuffer[i].portionTimes[j]));

    // Fix catID (8 bytes)
    NTOHLL((uint8_t*)(&updateBuffer[i].catID));
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
