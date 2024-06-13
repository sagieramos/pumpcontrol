#include "main.h"

TaskHandle_t blinkTaskHandle = NULL;

void blink(unsigned int timeOn, unsigned int timeOff) {
  digitalWrite(LED_BUILTIN, HIGH);
  vTaskDelay(timeOn / portTICK_PERIOD_MS);
  digitalWrite(LED_BUILTIN, LOW);
  vTaskDelay(timeOff / portTICK_PERIOD_MS);
}

// Task to handle blinking
void taskBlink(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    blink(100, 400);
    // stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("Blink Task High Water Mark: ");
    // Serial.println(stackHighWaterMark);
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  }
}
