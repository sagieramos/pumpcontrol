#include "main.h"
#include "routes.h"

void stackMonitor(void *pvParameter);

AsyncWebServer server(80);

void setup() {
  DEBUG_SERIAL_BEGIN(115200);

  if (!SPIFFS.begin()) {
    DEBUG_SERIAL_PRINTLN("Failed to mount SPIFFS");
    return;
  }
  DEBUG_SERIAL_PRINTLN("SPIFFS mounted successfully");

  int8_t count = 5;

  DEBUG_SERIAL_PRINTLN("Starting in 5 seconds... ");

  while (count >= 0) {
    DEBUG_SERIAL_PRINT("\r");
    DEBUG_SERIAL_PRINT(count);
    DEBUG_SERIAL_PRINT(" ");
    delay(1000);

    count--;
  }

  DEBUG_SERIAL_PRINTLN();

  pinMode(LED_BUILTIN, OUTPUT);
  EEPROM.begin(EEPROM_SIZE_CTL);

  // Initialize control data
  setupPumpControl();

  // Setup WiFi AP and DNS
  setupWifiAP();

  xTaskCreate(stackMonitor, "Stack monitor", 2560, NULL, 4, NULL);

  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "akowe.org", apIP);
  dnsServer.start(DNS_PORT, "www.akowe.org", apIP);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(604800);

  // Serve static files with appropriate MIME types and CORS headers
  // And routes
  for (int i = 0; i < numPaths; ++i) {
    server.on(paths[i], HTTP_GET,
              [path = paths[i],
               contentType = contentTypes[i]](AsyncWebServerRequest *request) {
                serveStaticFile(request, path, contentType);
              });
  }

  // Handle other requests including login, logout, and default routes
  server.on("*", HTTP_ANY,
            [](AsyncWebServerRequest *request) { handleRequest(request); });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }

void stackMonitor(void *pvParameter) {
  TaskHandle_t taskHandles[] = {dnsTaskHandle, blinkTaskHandle, runMachineTask};
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
