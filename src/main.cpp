#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "sensors.h"

#define WAKE_PIN GPIO_NUM_4

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
    {"/_lbundle.js", APP_JS},       {"/_login.js", APP_JS},
    {"/_login.css", TEXT_CSS},      {"/favicon.ico", "image/x-icon"},
    {"/logo.svg", IMAGE_SVG},       {"/warning.svg", IMAGE_SVG},
    {"/_dbundle.js", APP_JS},       {"/_dashboard.js", APP_JS},
    {"/_dashboard.css", TEXT_CSS},  {"/reset.css", TEXT_CSS},
    {"/217._dbundle.js", APP_JS},   {"/logout.svg", IMAGE_SVG},
    {"/828._dbundle.js", APP_JS},   {"/939._dbundle.js", APP_JS},
    {"/680._dbundle.js", APP_JS},   {"/27._dbundle.js", APP_JS},
    {"/notify.mp3", AUDIO_MP3},     {"/next-gray.svg", IMAGE_SVG},
    {"/next-white.svg", IMAGE_SVG}, {"/cancel.svg", IMAGE_SVG},
    {"/fb.svg", IMAGE_SVG},         {"/ig.svg", IMAGE_SVG},
    {"/in.svg", IMAGE_SVG},         {"/tiktok.svg", IMAGE_SVG},
    {"/x.svg", IMAGE_SVG}};

const int numPaths = sizeof(staticFiles) / sizeof(staticFiles[0]);

// Interrupt service routine for the wake pin
void IRAM_ATTR handleSleepWake() {
  LOG_LN("Going to sleep");
  esp_sleep_enable_ext0_wakeup(WAKE_PIN, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_light_sleep_start();
}

#define FLOAT_SWITCH 1

void IRAM_ATTR handleFloatSwitch() {
  LOG_LN("Float switch triggered");
  xTaskNotify(runMachineTaskHandle, FLOAT_SWITCH, eSetValueWithOverwrite);
}
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

  pinMode(WAKE_PIN, INPUT_PULLUP);
  attachInterrupt(WAKE_PIN, handleSleepWake, FALLING);

  pinMode(LED_BUILTIN, OUTPUT);
  if (xTaskCreatePinnedToCore(send_voltage_task, "SendVoltageTask", 4096, NULL,
                              2, &sendVoltageTask, 1) != pdPASS) {
    LOG_LN("Failed to create send voltage task");
  } else {
    LOG_LN("Send voltage task created successfully");
  }

  if (xTaskCreatePinnedToCore(runMachineTask, "Pump Controller", 4096, NULL, 1,
                              &runMachineTaskHandle, 1) != pdPASS) {
    LOG_LN("Failed to create pump controller task");
  } else {
    LOG_LN("Pump controller task created successfully");
  }

  if (xTaskCreatePinnedToCore(checkSignalTask, "Check Signal", 4096, NULL, 1,
                              &checkSignalHandle, 1) != pdPASS) {
    LOG_LN("Failed to create check signal task");
  } else {
    LOG_LN("Check signal task created successfully");
  }

  if (xTaskCreatePinnedToCore(powerControl, "Power Task", 4096, NULL, 3,
                              &powerControlTask, 1) != pdPASS) {
    LOG_LN("Failed to create power task");
  } else {
    LOG_LN("Power task created successfully");
  }

  if (xTaskCreatePinnedToCore(readPzemTask, "readPzem Task", 4096, NULL, 4,
                              &readPzemTaskHandle, 1) != pdPASS) {
    LOG_LN("Failed to readPzem Task");
  } else {
    LOG_LN("readPzem Task created successfully");
  }

  if (xTaskCreatePinnedToCore(msgGatewayTask, "Message Gateway Task", 4096,
                              NULL, 2, &msgGatewayTaskHandle, 1) != pdPASS) {
    LOG_LN("Failed to create Message Gateway Task");
  } else {
    LOG_LN("Message Gateway Task created successfully");
  }
  /*
    if (xTaskCreatePinnedToCore(stackMonitor, "stack Monitor", 4096, NULL, 3,
                                &readPzemTaskHandle, 1) != pdPASS) {
      LOG_LN("Failed to stack Monitor Task");
    } else {
      LOG_LN("stack Monitor Task created successfully");
    }
   */

  setupWifiAP();

  LOG_F("Access Point IP Address: %s\n", local_IP.toString().c_str());

  dnsServer.start(DNS_PORT, "*", local_IP);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(604800);

  // Handle WebSocket events
  ws.onEvent(onWsEvent);
  server.addHandler(&ws).setFilter([](AsyncWebServerRequest *request) {
    if (strcmp(request->url().c_str(), "/ws") == 0) {
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

  server.on("*", HTTP_ANY,
            [](AsyncWebServerRequest *request) { handleRequest(request); });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
