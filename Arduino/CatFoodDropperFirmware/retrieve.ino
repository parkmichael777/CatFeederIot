// Prints contents of updateBuffer in human-readable form in VERBOSE_MODE.
void printUpdateBuffer() {
  for (int i = 0; i < NUM_CATS; ++i) {
    if (updateBuffer[i].inUse == 0)
      continue;
    
    verbosePrint("Cat Profile " + String(i + 1), NULL);
    verbosePrint("Max Rate", updateBuffer[i].maxRate);
    verbosePrint("Portion Grams", updateBuffer[i].portionGrams);
    verbosePrint("Num Portions", updateBuffer[i].numPortions);

    for (int j = 0; j < updateBuffer[i].numPortions; ++j)
      verbosePrint("P" + String(j), updateBuffer[i].portionTimes[j]);

    verbosePrint("Cat ID", updateBuffer[i].catID);

    verbosePrint(NULL, NULL);
  }
}

// ntohl/htonl function that works with floats
void NTOHL(uint8_t *bytes) {
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

// ntohll/htonll function that works with unsigned long long (8 bytes)
void NTOHLL(uint8_t *bytes) {
  uint8_t b0, b1, b2, b3, b4, b5, b6, b7;

  b0 = bytes[0];
  b1 = bytes[1];
  b2 = bytes[2];
  b3 = bytes[3];
  b4 = bytes[4];
  b5 = bytes[5];
  b6 = bytes[6];
  b7 = bytes[7];

  bytes[0] = b7;
  bytes[1] = b6;
  bytes[2] = b5;
  bytes[3] = b4;
  bytes[4] = b3;
  bytes[5] = b2;
  bytes[6] = b1;
  bytes[7] = b0;
}

// Contacts server and sends get request for CatProfiles.
// If update available, populates updateBuffer and raises update flag.
// Returns 0 on success and -1 on failure.
int retrieveCatProfiles() {
  WiFiClient c;
  HttpClient client(c, SERVER_IP, SERVER_PORT);

  // Request CatProfiles
  client.beginRequest();
  client.get("CatProfiles");
  client.sendHeader("Cat-Profile-Version", String(catProfileVersion));
  client.endRequest();

  delay(1000);

  /* Fail if no response recv after 1 second:
   * - Init will call this fn again if no request found.
   * - Normal operation will ignore this failure since it already polls periodically.
   */
  if (client.available() == 0)
    return -1;

  int status = client.responseStatusCode();

  // Update profiles if new one received. 
  switch (status) {
    case 200:
      return handle200OK(client);
    case 204:
      return handle204NoContent();
    default:
      return handleStub(status);
  }
}
