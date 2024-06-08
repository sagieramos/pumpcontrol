#include "main.h"

TaskHandle_t blinkTaskHandle;

void taskRun(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("High water mark: ");
    Serial.println(uxHighWaterMark);

    vTaskDelay(pdMS_TO_TICKS(10000));
    vTaskSuspend(blinkTaskHandle);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(10000));
    vTaskResume(blinkTaskHandle);
  }
}