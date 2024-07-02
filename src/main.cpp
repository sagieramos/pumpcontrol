#include "main.h"
#include "routes.h"

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
  runPumpControl();

  // Setup WiFi AP and DNS
  setupWifiAP();

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
