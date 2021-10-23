#ifndef CATPROFILE_H
#define CATPROFILE_H

/* Server Structs */
typedef struct __attribute__((__packed__)) {
    uint8_t inUse;                          // Bool if this profile is active.
    float maxRate;                          // Max rate in grams/min.
    uint8_t portionGrams;                   // Weight dispensed in one portion.
    uint8_t numPortions;                    // Number of active indices in portionTimes.
    uint32_t portionTimes[NUM_PORTIONS];    // Time in minutes since 00:00 relative to UTC.
} catProfileServer;

/* Device Structs */
typedef struct {
    uint32_t amountDispensed;
    uint32_t portionDuration;
} bowlData;

typedef struct __attribute__((__packed__)) {
    uint8_t inUse;                          // Bool if this profile is active.
    float maxRate;                          // Max rate in grams/min.
    uint8_t portionGrams;                   // Weight dispensed in one portion.
    uint8_t numPortions;                    // Number of active indices in portionTimes.
    uint32_t portionTimes[NUM_PORTIONS];    // Time in minutes since 00:00 relative to UTC.

    // Visual aid:
    // Time axis |----Feeding Period----|---Waiting Period---|   |--Next Feeding Period--|--so on...
    //           ^StartISR              ^EndISR                  ^StartISR               ^EndISR

    uint8_t canEat;                         // Indicates whether in feeding period.
    uint8_t inProgress;                     // Indicates whether cat started current feeding period.
    uint8_t isComplete;                     // Indicates whether cat finished alloted portion for
                                            // this feeding period.

    volatile uint8_t timeEINTR;             // Indicates an interrupt occurred.
    esp_timer_handle_t timerHandle;

    volatile uint8_t dataFlag;              // Indicates bowlData is ready to send for this portion.
    bowlData data;
    SemaphoreHandle_t dataLock;
} catProfile;

#endif
