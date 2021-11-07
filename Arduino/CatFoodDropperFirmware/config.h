#ifndef CONFIG_H
#define CONFIG_H

/* Cat Bowl Config */
#define NUM_CATS      3               // Max num of supported cats. 
                                      // Changing NUM_CATS requires manual additions to intrArgs :(
#define NUM_PORTIONS  5               // Max num of schedulable portions.
#define FEED_PERIOD   1 * (60000)     // Number of ms portion is available for after it starts.
#define POLL_PERIOD   60 * (1000000)  // Number of us until bowl polls server for catProfile updates.
#define DISP_PERIOD   60 * (1000000)  // Number of us each dispensing period within a portion lasts.

/* WiFi and Server Config */
#define EDU_WIFI      0               // Whether we are connecting to IllinoisNet or not.

#if EDU_WIFI
#define SSID          "IllinoisNet_Guest"
#define SERVER_IP     "10.195.29.164"
#define SERVER_PORT   8000
#else
#define SSID          "NETGEAR13"
#define PWD           "largewater374"
#define SERVER_IP     "192.168.1.3"
#define SERVER_PORT   8000
#endif

/* Constants */
#define DAY           86400000        // Number of milliseconds in a day.
#define RFID_DATA     10              // Number of data bytes in RFID packet

/* Hardware Pins */
#define RX_PIN        16              // UART RX/TX
#define TX_PIN        17

#define LED           13              // Debug LED

#define RFID_NEARBY   21              // RFID

#define CELL_DOUT     23              // Load cell
#define CELL_SCLK     22 
#define CELL_PWDN     14
#define CELL_SPEED    32
#define CELL_CALIB    -7050.0         // Default provided by Sparkfun; TODO: tune to 1g.

/* Debug Config */
#define DEBUG_MODE    1               // Enable/Disable extra logging.
#define VERBOSE_MODE  1               // Enable/Disable extra extra logging.

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
