void printProfileBuffer() {
  for (int i = 0; i < NUM_CATS; ++i) {
    if (profileBuffer[i].inUse == 0)
      continue;

    verbosePrint("Cat Profile " + String(i + 1), NULL);
    verbosePrint("Max Rate", profileBuffer[i].maxRate);
    verbosePrint("Portion Grams", profileBuffer[i].portionGrams);
    verbosePrint("Num Portions", profileBuffer[i].numPortions);

    for (int j = 0; j < profileBuffer[i].numPortions; ++j)
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

// Register next timer interrupt for passed in profile.
void updateState(catProfile *p) {
  esp_timer_stop(p->timerHandle);

  p->timeEINTR = 0;

  time_t currTime = getTimeUTC();
  
  if (currTime < p->portionTimes[0]) {
    // TODO: Register interrupt for first portion.
  }
  else if (currTime > p->portionTimes[p->numPortions - 1]) {
    // TODO: Register interrupt for rollover or last portion end.
  }
  else {
    time_t nextTime, prevTime;

    // Find which scheduled times border the currTime.
    for (int i = 0; i < p->numPortions; ++i) {
      if (currTime > p->portionTimes[i])
        continue;

      nextTime = p->portionTimes[i];
      prevTime = p->portionTimes[i - 1];
    }
    
    if (currTime < prevTime + FEED_PERIOD) {
      // Register end time interrupt (i.e. Update state to be in feeding period).
      p->canEat = 1;

      debugPrint("Registered end time in", prevTime + FEED_PERIOD - currTime);

      assert(prevTime + FEED_PERIOD > currTime);
      esp_timer_start_once(p->timerHandle, (prevTime + FEED_PERIOD - currTime) * 1000000);
    }
    else {
      // Register start time interrupt (i.e. Update state to be in waiting period).
      p->canEat = 0;
      p->inProgress = 0;
      p->isComplete = 0;

      xSemaphoreTake(p->dataLock, portMAX_DELAY);
      p->dataFlag = 1;
      xSemaphoreGive(p->dataLock);

      debugPrint("Registered start time in", nextTime - currTime);

      assert(nextTime > currTime);
      esp_timer_start_once(p->timerHandle, (nextTime - currTime) * 1000000);
    }
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

  // Update state
  for (int i = 0; i < NUM_CATS; ++i) {
    if (profileBuffer[i].inUse == 0)
      continue;
    
    updateState(&profileBuffer[i]);
  }

  printProfileBuffer();
}
