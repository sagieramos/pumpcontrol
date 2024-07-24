#include "main.h"
#include "network.h"
#include "pump_control.h"

struct StaticFile {
  const char *path;
  const char *contentType;
};

// Array of StaticFile structures
const StaticFile staticFiles[] = {{"/_lbundle.js", "application/javascript"},
                                  {"/_lindex.js", "application/javascript"},
                                  {"/_lstyles.css", "text/css"},
                                  {"/favicon.ico", "image/x-icon"},
                                  {"/logo.svg", "image/svg+xml"},
                                  {"/warning.svg", "image/svg+xml"},
                                  {"/_dbundle.js", "application/javascript"},
                                  {"/_dashboard.js", "application/javascript"},
                                  {"/_dashboard.css", "text/css"}};

const int numPaths = sizeof(staticFiles) / sizeof(staticFiles[0]);

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

  xTaskCreate(runMachine, "Pump Controller", 4096, NULL, 1, &runMachineTask);

  // Setup WiFi AP and DNS
  setupWifiAP();

  xTaskCreate(stackMonitor, "Stack monitor", 2560, NULL, 4, NULL);

  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "akowe.org", apIP);
  dnsServer.start(DNS_PORT, "www.akowe.org", apIP);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(604800);

  // Handle WebSocket events
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve static files with appropriate MIME types and CORS headers
  // And routes
  for (int i = 0; i < numPaths; i++) {
    server.on(staticFiles[i].path, HTTP_GET,
              [file = staticFiles[i]](AsyncWebServerRequest *request) {
                DEBUG_SERIAL_PRINTF("Serving path: %s with content type: %s\n",
                                    file.path, file.contentType);
                serveStaticFile(request, file.path, file.contentType);
              });
  }

  // Handle other requests including login, logout, and default routes
  server.on("*", HTTP_ANY,
            [](AsyncWebServerRequest *request) { handleRequest(request); });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
