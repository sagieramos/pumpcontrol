#include "dev_or_prod.h"

uint32_t getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  uint32_t currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}

void check_and_resume_task(TaskHandle_t task, bool resumeTask) {
  if (task != NULL) {
    eTaskState taskState = eTaskGetState(task);

    if (resumeTask) {
      if (taskState == eSuspended) {
        vTaskResume(task);
        DEBUG_SERIAL_PRINTF("Resumed task with name: %s\n",
                            pcTaskGetTaskName(task));
      } else {
        DEBUG_SERIAL_PRINTF("Task with name: %s is already running\n",
                            pcTaskGetTaskName(task));
      }
    } else {
      if (taskState != eSuspended) {
        vTaskSuspend(task);
        DEBUG_SERIAL_PRINTF("Suspended task with name: %s\n",
                            pcTaskGetTaskName(task));
      } else {
        DEBUG_SERIAL_PRINTF("Task with name: %s is already suspended\n",
                            pcTaskGetTaskName(task));
      }
    }
  } else {
    DEBUG_SERIAL_PRINTLN("Task is NULL");
  }
}
