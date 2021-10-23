#ifndef CONFIG_H
#define CONFIG_H

/* Cat Bowl Config */
#define NUM_CATS      3            // Max num of supported cats. 
                                   // Note: Changing NUM_CATS requires manual additions to intrArgs :(
#define NUM_PORTIONS  5            // Max num of schedulable portions.
#define FEED_PERIOD   (1 * 60000)  // Number of ms portion is available for after it starts.

/* WiFi and Server Config */
#define SSID          "NETGEAR13"
#define PWD           "largewater374"
#define SERVER_IP     "192.168.1.3"
#define SERVER_PORT   8000

/* Constants */
#define DAY 86400000     // Number of milliseconds in a day.

/* Debug Config */
#define DEBUG_MODE    1        // Enable/Disable extra logging.
#define VERBOSE_MODE  1        // Enable/Disable extra extra logging.

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
