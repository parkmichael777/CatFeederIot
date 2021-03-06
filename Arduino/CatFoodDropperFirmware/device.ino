// Handles UART events using event queue. Boilerplate borrowed from official example.
// https://github.com/espressif/esp-idf/tree/5f0c8f5/examples/peripherals/uart/uart_events
void uartHandler(void *pvParameters) {
  uart_event_t event;
  uint8_t buf[16] = {0};

  for(;;) {
    if(xQueueReceive(uartQueue, (void *)&event, (portTickType)portMAX_DELAY)) {
      switch(event.type) { 
        case UART_DATA:
        {
          uint64_t pendingCatID = 0;

          assert(event.size <= 16);

          uart_read_bytes(UART_NUM_2, buf, event.size, portMAX_DELAY);

          uint8_t *data = buf + 1;    // Skip STX byte in RFID packet frame.

          // Convert ASCII to actual number. (e.g. 'ABCD' -> 0xABCD)
          for (int i = 0; i < RFID_DATA; ++i) {
            uint8_t curr = (uint8_t)(~buf[i]);    // Data comes in inverted.

            // Convert ASCII to hex:
            if (curr > '0' || curr < '9')
              curr -= '0';
            else if (curr > 'A' || curr < 'F')
              curr -= 'A';
            else {
              debugPrint("Error: invalid ASCII sent via UART", NULL);
              assert(false);
            }

            pendingCatID <<= 4;
            pendingCatID |= curr;
          }

          debugPrint("CatID detected", pendingCatID);

          xSemaphoreTake(catIDLock, portMAX_DELAY);
          catID = pendingCatID;
          xSemaphoreGive(catIDLock);          
          break;
        }
        case UART_FIFO_OVF:
          debugPrint("Received Event FIFO_OVF", NULL);
          break;
        case UART_BUFFER_FULL:
          debugPrint("Received Event BUFFER_FULL", NULL);
          break;
        case UART_BREAK:
          debugPrint("Received Event BREAK", NULL);
          break;
        case UART_PARITY_ERR:
          debugPrint("Received Event PARITY_ERR", NULL);
          break;
        case UART_FRAME_ERR:
          debugPrint("Received Event FRAME_ERR", NULL);
          break;
        case UART_PATTERN_DET:
          debugPrint("Received Event PATTERN_DET", NULL);
          break;
        default:
          debugPrint("Received Event", event.type);
          break;
      }
    }
  }
}

// Check all profiles for interrupts.
void checkEINTR() {
  // Test interrupt scheduling
  if (profileBuffer[0].timeEINTR)
    updateState(&profileBuffer[0]);
  else if (profileBuffer[1].timeEINTR)
    updateState(&profileBuffer[1]);
  else if (profileBuffer[2].timeEINTR)
    updateState(&profileBuffer[2]);
}

// Accesses catID and traces it back to a profileBuffer.
int nearbyCat() {
  uint64_t id = 0;

  if (digitalRead(RFID_NEARBY) == LOW)
    return -1;

  xSemaphoreTake(catIDLock, portMAX_DELAY);
  id = catID;
  xSemaphoreGive(catIDLock);
  
  // Scan profileBuffer and retrieve corresponding profile.
  for (uint32_t i = 0; i < NUM_CATS; ++i) {
    if (id == profileBuffer[i].catID)
      return i;
  }

  return -1;
}
