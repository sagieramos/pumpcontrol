#include "main.h"

DNSServer dnsServer; 
// print the size of dnsServer

TaskHandle_t dnsTaskHandle = NULL;

void dnsTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    /*     UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        Serial.print("High water mark: ");
        Serial.println(uxHighWaterMark); */
    dnsServer.processNextRequest();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}