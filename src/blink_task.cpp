#include "main.h"

void blink(unsigned int timeOn, unsigned int timeOff) {
  vTaskDelay(pdMS_TO_TICKS(timeOff));
  digitalWrite(LED_BUILTIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(timeOn));
  digitalWrite(LED_BUILTIN, LOW);
}

// Task to handle blinking
void taskBlink(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    blink(500, 500);
  }
}
