#include "main.h"
#include "pump_control.h"
#include "sensors.h"

void stackMonitor(void *pvParameter) {
  TaskHandle_t taskHandles[] = {dnsTaskHandle, blinkTaskHandle, runMachineTask,
                                sendVoltageTask};
  const int numTasks = sizeof(taskHandles) / sizeof(TaskHandle_t);

  for (;;) {
    DEBUG_SERIAL_PRINTF("STACK MONITOR: Time: %lu ms\n", getCurrentTimeMs());

    for (int i = 0; i < numTasks; i++) {
      TaskHandle_t handle = taskHandles[i];
      const char *taskName = pcTaskGetName(handle);

      if (handle != NULL) {
        eTaskState taskState = eTaskGetState(handle);
        if (taskState != eSuspended) {
          UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(handle);
          UBaseType_t taskPriority = uxTaskPriorityGet(handle);

          DEBUG_SERIAL_PRINTF(
              "%s Task: %u (Stack High Water Mark), %u (Priority)\n", taskName,
              stackHighWaterMark, taskPriority);
        } else {
          DEBUG_SERIAL_PRINTF("%s Task is suspended\n", taskName);
        }
      } else {
        DEBUG_SERIAL_PRINTF("%s Task handle is NULL\n", taskName);
      }
    }
    DEBUG_SERIAL_PRINTLN();

    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}
