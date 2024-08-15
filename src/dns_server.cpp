#include "main.h"
#include "network.h"

DNSServer dnsServer;

TaskHandle_t dnsTaskHandle = NULL;

void dnsTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    dnsServer.processNextRequest();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}