#include "main.h"

TaskHandle_t blinkTaskHandle = NULL;

void blink(unsigned int timeOn, unsigned int timeOff) {
  digitalWrite(LED_BUILTIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(timeOn));
  digitalWrite(LED_BUILTIN, LOW);
  vTaskDelay(pdMS_TO_TICKS(timeOff));
}

// Task to handle blinking
void taskBlink(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    blink(100, 400);
    vTaskDelay(pdMS_TO_TICKS(10));
    // stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("Blink Task High Water Mark: ");
    // Serial.println(stackHighWaterMark);
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  }
}
