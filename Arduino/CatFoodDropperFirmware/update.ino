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
    verbosePrint("Is Comp", profileBuffer[i].isComplete);
    verbosePrint("Amt Eaten", profileBuffer[i].amountEaten);

    verbosePrint("Cat ID", profileBuffer[i].catID);

    verbosePrint(NULL, NULL);
  }
}

// Returns number of milliseconds passed since CST (UTC-6) 00:00 with DST adjustment.
TIME_T getTime() {
  struct timeval tv;
  struct tm t;

  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &t);

  return (TIME_T)t.tm_hour * 3600000 + (TIME_T)t.tm_min * 60000 + 
         (TIME_T)t.tm_sec * 1000 + ((TIME_T)tv.tv_usec / 1000);
}

// Update state to be in feeding period + register end time intr.
void updateToFeeding(catProfile *p, TIME_T currTime, TIME_T prevTime) {
  p->canEat = 1;

  /* If update changes state from waiting->feeding, isComplete and inProgress already set.
   * If update changes state from feeding->feeding, isComplete and inProgress should not be changed
   *  except in the case that the owner increased the portionGrams.
   */

  // If owner increased portionGrams, allow cat to eat again.
  if ((p->amountEaten < p->portionGrams) && (p->isComplete == 1)) {
    p->isComplete = 0;
    p->inProgress = 1;
  }

  verbosePrint("P+FDTime", prevTime + FEED_PERIOD);
  verbosePrint("CurrTime", currTime);
  verbosePrint("Registered end time in", prevTime + FEED_PERIOD - currTime);

  assert(prevTime + FEED_PERIOD > currTime);
  esp_timer_start_once(p->timerHandle, (prevTime + FEED_PERIOD - currTime) * 1000);
}

// Upodate state to be in waiting period + register start time intr.
void updateToWaiting(catProfile *p, TIME_T currTime, TIME_T nextTime) {
  p->canEat = 0;
  p->inProgress = 0;
  p->isComplete = 0;
  p->amountEaten = 0;

  verbosePrint("NextTime", nextTime);
  verbosePrint("CurrTime", currTime);
  verbosePrint("Registered start time in", nextTime - currTime);
  
  assert(nextTime > currTime);
  esp_timer_start_once(p->timerHandle, (nextTime - currTime) * 1000);
}

// Register next timer interrupt for passed in profile.
void updateState(catProfile *p) {
  esp_timer_stop(p->timerHandle);

  p->timeEINTR = 0;

  TIME_T currTime = getTime();
  
  if (currTime < p->portionTimes[0]) {
    debugPrint("Type 1 Waiting", NULL);
    
    // System initialized with currTime < all portionTimes.
    TIME_T nextTime = p->portionTimes[0];
    updateToWaiting(p, currTime, nextTime);
  }
  else if (currTime >= p->portionTimes[p->numPortions - 1]) {
    // System is has consumed last portionTime start.
    
    // Set prevTime as last portionTime and nextTime as rollover/first portionTime.
    TIME_T prevTime = p->portionTimes[p->numPortions - 1];
    TIME_T nextTime = p->portionTimes[0] + DAY;

    // Initialize state and register based on which period we are in.
    if (currTime < prevTime + FEED_PERIOD) {
      debugPrint("Type 2 Feeding", NULL);
      updateToFeeding(p, currTime, prevTime);
    }
    else {
      debugPrint("Type 2 Waiting", NULL);
      updateToWaiting(p, currTime, nextTime);
    } 
  }
  else {
    // System is somewhere in the middle of portionTimes.
    TIME_T nextTime, prevTime;

    // Find which portionTimes surround the currTime
    for (int i = 0; i < p->numPortions; ++i) {
      if (currTime >= p->portionTimes[i])
        continue;

      nextTime = p->portionTimes[i];
      prevTime = p->portionTimes[i - 1];
      break;
    }

    // Initialize state and register based on which period we are in.
    if (currTime < prevTime + FEED_PERIOD) {
      debugPrint("Type 3 Feeding", NULL);
      updateToFeeding(p, currTime, prevTime);
    }
    else {
      debugPrint("Type 3 Waiting", NULL);
      updateToWaiting(p, currTime, nextTime);
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
