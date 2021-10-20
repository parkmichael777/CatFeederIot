void printProfileBuffer() {
  for (int i = 0; i < NUM_CATS; ++i) {
    verbosePrint("Cat Profile " + String(i + 1), NULL);
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

// Returns number of seconds passed since UTC 00:00.
time_t getTimeUTC() {
  time_t sec;
  struct tm t;

  time(&sec);
  localtime_r(&sec, &t);
  return t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec;
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
