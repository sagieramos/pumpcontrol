#include "main.h"
#include "pump_control.h"
#include "sensors.h"

void stackMonitor(void *pvParameter) {
  TaskHandle_t taskHandles[] = {dnsTaskHandle, blinkTaskHandle, runMachineTask,
                                checkSignalTask, sendVoltageTask};
  const int numTasks = sizeof(taskHandles) / sizeof(TaskHandle_t);

  for (;;) {
    LOG_F("STACK MONITOR: Time: %lu ms\n", getCurrentTimeMs());

    for (int i = 0; i < numTasks; i++) {
      TaskHandle_t handle = taskHandles[i];
      const char *taskName = pcTaskGetName(handle);

      if (handle != NULL) {
        eTaskState taskState = eTaskGetState(handle);
        if (taskState != eSuspended) {
          UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(handle);
          UBaseType_t taskPriority = uxTaskPriorityGet(handle);

          LOG_F("%s Task: %u (Stack High Water Mark), %u (Priority)\n",
                taskName, stackHighWaterMark, taskPriority);
        } else {
          LOG_F("%s Task is suspended\n", taskName);
        }
      } else {
        LOG_F("%s Task handle is NULL\n", taskName);
      }
    }
    LOG_LN();

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
