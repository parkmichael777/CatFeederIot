// Prints contents of updateBuffer in human-readable form in VERBOSE_MODE.
void printUpdateBuffer() {
//  for (int i = 1; i <= NUM_CATS; ++i) {
//    verbosePrint("Cat Profile " + String(i), NULL);
//    verbosePrint("Max Rate", updateBuffer[i].maxRate);
//    verbosePrint("Portion Grams", updateBuffer[i].portionGrams);
//    verbosePrint("Num Portions", updateBuffer[i].numPortions);
//
//    for (int j = 0; j < NUM_PORTIONS; ++j)
//      verbosePrint("P" + String(j), updateBuffer[i].portionTimes[j]);
//
//    verbosePrint(NULL, NULL);
//  }
}

void printProfileBuffer() {
  for (int i = 1; i <= NUM_CATS; ++i) {
    verbosePrint("Cat Profile " + String(i), NULL);
    verbosePrint("Max Rate", profileBuffer[i].maxRate);
    verbosePrint("Portion Grams", profileBuffer[i].portionGrams);
    verbosePrint("Num Portions", profileBuffer[i].numPortions);

    for (int j = 0; j < NUM_PORTIONS; ++j)
      verbosePrint("P" + String(j), profileBuffer[i].portionTimes[j]);

    verbosePrint("Can Eat", profileBuffer[i].canEat);
    verbosePrint("In Prog", profileBuffer[i].inProgress);

    verbosePrint("Amt Disp", profileBuffer[i].data.amountDispensed);
    verbosePrint("Ptn Drtn", profileBuffer[i].data.portionDuration);

    verbosePrint(NULL, NULL);
  }
}

// ntohl/htonl function that works with floats
void NetworkToHostL(uint8_t *bytes) {
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

// Contacts server and sends get request for CatProfiles.
// If update available, populates updateBuffer and raises update flag.
void retrieveCatProfiles() {
  WiFiClient c;
  HttpClient client(c, SERVER_IP, SERVER_PORT);

  // Request CatProfiles
  client.beginRequest();
  client.get("CatProfiles");
  client.sendHeader("Cat-Profile-Version", String(catProfileVersion));
  client.endRequest();

  int status = client.responseStatusCode();

  // Update profiles if new one received. 
  if (status == 200) {
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
  
    // Fix byte order of maxRate.
    for (int i = 0; i < NUM_CATS; ++i)
      NetworkToHostL((uint8_t*)(&updateBuffer[i]));
  
    // Raise update flag
    updateFlag = 1;
  
    xSemaphoreGive(updateLock);

    client.stop();

    printUpdateBuffer();
      
  }
  else if (status == 204) {
    debugPrint("No Content", NULL);
  }
  else {
    debugPrint("Error: Unhandled Response Code", status);
    while(true);
  }
}

// Checks whether update is available (update flag) and if so,
// copies pending updateBuffer into 'profile,' the usable catProfile array.
void updateCatProfiles() {
  // Copy updates.
  xSemaphoreTake(updateLock, portMAX_DELAY);

  if (updateFlag == 0) {
    xSemaphoreGive(updateLock);
    return;
  }

  for (int i = 0; i < NUM_CATS; ++i)
    memcpy(profileBuffer + i, updateBuffer + i, sizeof(catProfileServer));

  updateFlag = 0;

  xSemaphoreGive(updateLock);

  // TODO: Initialize canEat and inProgress

  // TODO: Register Interrupts

  printProfileBuffer();
}
