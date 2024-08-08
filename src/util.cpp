#include "dev_or_prod.h"

uint32_t getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  uint32_t currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}

bool check_and_resume_task(TaskHandle_t task) {
  if (task != NULL) {
    eTaskState taskState = eTaskGetState(task);
    if (taskState == eSuspended) {
      vTaskResume(task);
      return true;
    }
  }
  return false;
}