#include "dev_or_prod.h"

unsigned long getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  unsigned long currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}

void check_and_resume_task(TaskHandle_t task, bool resumeTask) {
  if (task != NULL) {
    eTaskState taskState = eTaskGetState(task);

    if (resumeTask) {
      if (taskState == eSuspended) {
        vTaskResume(task);
        LOG_F("Resumed task with name: %s\n", pcTaskGetTaskName(task));
      } else {
        LOG_F("Task with name: %s is already running\n",
              pcTaskGetTaskName(task));
      }
    } else {
      if (taskState != eSuspended) {
        vTaskSuspend(task);
        LOG_F("Suspended task with name: %s\n", pcTaskGetTaskName(task));
      } else {
        LOG_F("Task with name: %s is already suspended\n",
              pcTaskGetTaskName(task));
      }
    }
  } else {
    LOG_LN("Task is NULL");
  }
}
