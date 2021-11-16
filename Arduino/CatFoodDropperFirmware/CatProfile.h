#ifndef CATPROFILE_H
#define CATPROFILE_H

/* Server Structs */
typedef struct __attribute__((__packed__)) {
    uint8_t inUse;                          // Bool if this profile is active.
    float maxRate;                          // Max rate in grams/min.
    float portionGrams;                     // Weight dispensed in one portion.
    uint8_t numPortions;                    // Number of active indices in portionTimes.
    TIME_T portionTimes[NUM_PORTIONS];      // Time in milliseconds since 00:00 relative to CST.
    uint64_t catID;                         // Holds 5 byte RFID tag number.
} catProfileServer;

/* Device Structs */
typedef struct __attribute__((__packed__)) {
    int profileIndex;           // Which cat does this data belong to?
    float amountEaten;          // How much food was eaten in the last minute.
    TIME_T timeStamp;           // Timestamp of when data was collected.
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
    //
    // The feeding period is split up into smaller 1 minute periods where the
    // dropper tops off the bowl to a max weight of maxRate.
    
    uint32_t amountEaten;                   // Grams of food eaten within this portion.
    uint8_t canEat;                         // Indicates whether in feeding period.
    uint8_t inProgress;                     // Indicates whether cat started current feeding period.
    uint8_t isComplete;                     // Indicates whether cat finished alloted portion for
                                            // this feeding period.

    volatile uint8_t timeEINTR;             // Indicates an interrupt occurred.
    esp_timer_handle_t timerHandle;
} catProfile;

#endif
