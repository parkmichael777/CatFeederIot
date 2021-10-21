// Prints contents of updateBuffer in human-readable form in VERBOSE_MODE.
void printUpdateBuffer() {
  for (int i = 0; i < NUM_CATS; ++i) {
    verbosePrint("Cat Profile " + String(i + 1), NULL);
    verbosePrint("Max Rate", updateBuffer[i].maxRate);
    verbosePrint("Portion Grams", updateBuffer[i].portionGrams);
    verbosePrint("Num Portions", updateBuffer[i].numPortions);

    for (int j = 0; j < NUM_PORTIONS; ++j)
      verbosePrint("P" + String(j), updateBuffer[i].portionTimes[j]);

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

  // Init will call this fn again if no request found.
  // Normal operation will ignore this failure since it already polls periodically.
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
