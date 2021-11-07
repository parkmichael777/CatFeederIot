#ifndef CATPROFILE_H
#define CATPROFILE_H

/* Server Structs */
typedef struct __attribute__((__packed__)) {
    uint8_t inUse;                          // Bool if this profile is active.
    float maxRate;                          // Max rate in grams/min.
    float portionGrams;                     // Weight dispensed in one portion.
    uint8_t numPortions;                    // Number of active indices in portionTimes.
    TIME_T portionTimes[NUM_PORTIONS];      // Time in minutes since 00:00 relative to CST.
    uint64_t catID;                         // Holds 5 byte RFID tag number.
} catProfileServer;

/* Device Structs */
typedef struct __attribute__((__packed__)) {
    int profileIndex;
    float amountDispensed;
    TIME_T timeStamp;
} dataPacket;

typedef struct __attribute__((__packed__)) {
    uint8_t inUse;                          // Bool if this profile is active.
    float maxRate;                          // Max rate in grams/min.
    float portionGrams;                     // Weight dispensed in one portion.
    uint8_t numPortions;                    // Number of active indices in portionTimes.
    TIME_T portionTimes[NUM_PORTIONS];      // Time in minutes since 00:00 relative to CST.
    uint64_t catID;                         // Holds 5 byte RFID tag number.

    // Visual aid:
    // Time axis |----Feeding Period----|---Waiting Period---|   |--Next Feeding Period--|--so on...
    //           ^StartISR              ^EndISR                  ^StartISR               ^EndISR

    uint32_t amountDispensed;               // Grams of food dispensed for current portion.
    uint8_t canEat;                         // Indicates whether in feeding period.
    uint8_t inProgress;                     // Indicates whether cat started current feeding period.
    uint8_t isComplete;                     // Indicates whether cat finished alloted portion for
                                            // this feeding period.

    volatile uint8_t timeEINTR;             // Indicates an interrupt occurred.
    esp_timer_handle_t timerHandle;
} catProfile;

#endif
