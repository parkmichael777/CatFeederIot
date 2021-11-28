#ifndef CONFIG_H
#define CONFIG_H

/* Cat Bowl Config */
#define NUM_CATS      3               // Max num of supported cats. 
                                      // Changing NUM_CATS requires manual additions to intrArgs :(
#define NUM_PORTIONS  5               // Max num of schedulable portions.
#define CELL_READS    2               // Number of readings to average in order to return one weight.
#define CELL_TARES    50              // Number of readings to average for device tare.
#define FEED_PERIOD   60 * MS_PER_MIN // Number of ms portion is available for after it starts.
#define POLL_PERIOD   60 * US_PER_SEC // Number of us until bowl polls server for catProfile updates.
#define DISP_PERIOD   60 * US_PER_SEC // Number of us each dispensing period within a portion lasts.

/* WiFi and Server Config */
#define EDU_WIFI      1               // Whether we are connecting to IllinoisNet or not.

#if EDU_WIFI
#define SSID          "IllinoisNet_Guest"
#define SERVER_IP     "10.195.29.164"
#define SERVER_PORT   8000
#else
#define SSID          "NETGEAR13"
#define PWD           "largewater374"
#define SERVER_IP     "192.168.1.5"
#define SERVER_PORT   8000
#endif

/* Debug Config */
#define DEBUG_MODE    1               // Enable/Disable extra logging.
#define VERBOSE_MODE  1               // Enable/Disable extra extra logging.

/* Constants */
#define DAY           86400000        // Number of milliseconds in a day.
#define MS_PER_MIN    60000           // Number of milliseconds in a minute.
#define US_PER_SEC    1000000         // Number of microseconds in a second.
#define RFID_DATA     10              // Number of data bytes in RFID packet

/* Hardware Pins */
#define RX_PIN        27              // UART RX/TX
#define TX_PIN        17

#define RFID_NEARBY   26              // RFID

#define CELL_DOUT     19              // Load cell
#define CELL_SCLK     21 
#define CELL_PWDN     18
#define CELL_SPEED    23
#define CELL_CALIB    -5990.0        // Tuned via a known control weight.

#define MOTOR_STBY    14
#define MOTOR_IN1     12
#define MOTOR_IN2     13

/* Types */
typedef uint64_t TIME_T;

// Print macro; activated via DEBUG_MODE.
#if DEBUG_MODE
#define debugPrint(tag, val) {             \
          if ((tag) == NULL)               \
            Serial.println();              \
          else {                           \
            Serial.print((tag));           \
            if ((val) == NULL)             \
              Serial.println();            \
            else {                         \
              Serial.print(": ");          \
              Serial.println((val));       \
            }                              \
          }                                \
        }
#else
#define debugPrint(x, y) {}
#endif

// Print macro; activated via VERBOSE_MODE.
#if VERBOSE_MODE
#define verbosePrint(tag, val) {           \
          if ((tag) == NULL)               \
            Serial.println();              \
          else {                           \
            Serial.print((tag));           \
            if ((val) == NULL)             \
              Serial.println();            \
            else {                         \
              Serial.print(": ");          \
              Serial.println((val));       \
            }                              \
          }                                \
        }
#else
#define verbosePrint(x, y) {}
#endif

#endif
