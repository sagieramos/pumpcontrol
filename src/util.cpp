#include "dev_or_prod.h"

uint32_t getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  uint32_t currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}