#ifndef CATPROFILE_H
#define CATPROFILE_H

/* Server Structs */
typedef struct __attribute__((__packed__)) {
    float maxRate;              // Max rate in grams/min.
    uint8_t portionGrams;       // Weight dispensed in one portion.
    uint8_t numPortions;        // Number of active indices in portionTimes.
    uint8_t portionTimes[5];    // Time in minutes since 00:00
} catProfileServer;

/* Device Structs */
typedef struct {
    uint32_t amountDispensed;
    uint32_t portionDuration;
} bowlData;

typedef struct __attribute__((__packed__)) {
    float maxRate;              // Max rate in grams/min.
    uint8_t portionGrams;       // Weight dispensed in one portion.
    uint8_t numPortions;        // Number of active indices in portionTimes.
    uint8_t portionTimes[5];    // Time in minutes since 00:00

    volatile uint8_t canEat;
    volatile uint8_t inProgress;
    
    portMUX_TYPE dataMux;
    bowlData data;
} catProfile;

#endif
