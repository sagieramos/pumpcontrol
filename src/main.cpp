#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "sensors.h"
/* #include "esp_task_wdt.h" */

struct StaticFile {
  const char *path;
  const char *contentType;
};

#define APP_JS "application/javascript"
#define TEXT_CSS "text/css"
#define IMAGE_SVG "image/svg+xml"
#define AUDIO_MP3 "audio/mpeg"

// Array of StaticFile structures
const StaticFile staticFiles[] = {
    {"/_lbundle.js", APP_JS},      {"/_login.js", APP_JS},
    {"/_login.css", TEXT_CSS},     {"/favicon.ico", "image/x-icon"},
    {"/logo.svg", IMAGE_SVG},      {"/warning.svg", IMAGE_SVG},
    {"/_dbundle.js", APP_JS},      {"/_dashboard.js", APP_JS},
    {"/_dashboard.css", TEXT_CSS}, {"/217._dbundle.js", APP_JS},
    {"/828._dbundle.js", APP_JS},  {"/939._dbundle.js", APP_JS},
    {"/680._dbundle.js", APP_JS},  {"/27._dbundle.js", APP_JS},
    {"/notify.mp3", AUDIO_MP3},
};

const int numPaths = sizeof(staticFiles) / sizeof(staticFiles[0]);

void setup() {
  LOG_BEGIN(115200);
  if (!SPIFFS.begin()) {
    LOG_LN("Failed to mount SPIFFS");
    return;
  }
  LOG_LN("SPIFFS mounted successfully");

  int8_t count = 5;

  LOG_LN("Starting in 5 seconds... ");

  while (count >= 0) {
    LOG_("\r");
    LOG_(count);
    LOG_(" ");
    delay(1000);

    count--;
  }

  LOG_LN();

  pinMode(LED_BUILTIN, OUTPUT);
  if (xTaskCreate(send_voltage_task, "SendVoltageTask", 4096, NULL, 2,
                  &sendVoltageTask) != pdPASS) {
    LOG_LN("Failed to create send voltage task");
  } else {
    LOG_LN("Send voltage task created successfully");
  }

  if (xTaskCreate(runMachine, "Pump Controller", 4096, NULL, 1,
                  &runMachineTask) != pdPASS) {
    LOG_LN("Failed to create pump controller task");
  } else {
    LOG_LN("Pump controller task created successfully");
  }

  if (xTaskCreate(checkSignal, "Check Signal", 4096, NULL, 1,
                  &checkSignalTask) != pdPASS) {
    LOG_LN("Failed to create check signal task");
  } else {
    LOG_LN("Check signal task created successfully");
  }

  // Setup WiFi AP and DNS
  setupWifiAP();

  // xTaskCreate(stackMonitor, "Stack monitor", 2560, NULL, 4, NULL);

  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "akowe.org", apIP);
  dnsServer.start(DNS_PORT, "www.akowe.org", apIP);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(604800);

  // Handle WebSocket events
  ws.onEvent(onWsEvent);
  server.addHandler(&ws).setFilter([](AsyncWebServerRequest *request) {
    String urlPath = request->url();
    LOG_F("URL Path: %s\n", urlPath.c_str());
    if (strcmp(urlPath.c_str(), "/ws") == 0) {
      return authSession(authClients, request, CHECK) == ACTIVE;
    }

    return false;
  });

  // Serve static files with appropriate MIME types and CORS headers
  // And routes
  for (int i = 0; i < numPaths; i++) {
    server.on(staticFiles[i].path, HTTP_GET,
              [file = staticFiles[i]](AsyncWebServerRequest *request) {
                LOG_F("Serving path: %s with content type: %s\n", file.path,
                      file.contentType);
                serveStaticFile(request, file.path, file.contentType);
              });
  }

  // Handle other requests including login, logout, and default routes
  server.on("*", HTTP_ANY,
            [](AsyncWebServerRequest *request) { handleRequest(request); });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
