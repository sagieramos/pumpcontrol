#include "main.h"

String getTaskInfo(TaskHandle_t taskHandle) {
  String taskInfo = "TASK: ";
  taskInfo += pcTaskGetName(taskHandle);
  taskInfo += ", High Watermark: ";
  taskInfo += String(uxTaskGetStackHighWaterMark(taskHandle));

  return taskInfo;
}