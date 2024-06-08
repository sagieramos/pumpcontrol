#include "main.h"

void taskRun(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("High water mark: ");
    // Serial.println(uxHighWaterMark);
  }
}