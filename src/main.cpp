#include "main.h"
#include <ESPAsyncWebServer.h>

const char htmlForm[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Form</title>
    </head>
    <body>
      <h1>ESP32 Form</h1>
      <form action="/submit" method="GET">
        <label for="input1">Input 1:</label>
        <input type="text" id="input1" name="input1"><br><br>
        <label for="input2">Input 2:</label>
        <input type="text" id="input2" name="input2"><br><br>
        <input type="submit" value="Submit">
      </form>
    </body>
    </html>
  )rawliteral";

AsyncWebServer server(80);

void setup() {
  DEBUG_SERIAL_BEGIN(115200);
  // Serial.begin(115200);
  // delete old config

  int count = 5;

  DEBUG_SERIAL_PRINTLN("Starting in 5 seconds... ");

  while (count >= 0) {
    DEBUG_SERIAL_PRINT("\r");
    DEBUG_SERIAL_PRINT(count);
    DEBUG_SERIAL_PRINT(" ");
    delay(1000);

    count--;
  }

  DEBUG_SERIAL_PRINTLN();

  // Examples of different ways to register wifi events;
  // these handlers will be called from another thread.
  pinMode(LED_BUILTIN, OUTPUT);

  // Create the blink task

  // xTaskCreate(taskRun, "Task Run", 2048, NULL, 1, NULL);
  // xTaskCreate(dnsTask, "DNS Task", 10000, NULL, 1, &dnsTaskHandle);
  // xTaskCreate(taskBlink, "Blink Task", 2048, NULL, 1, &blinkTaskHandle);
  // xTaskCreate(dnsTask, "DNS Task", 10000, NULL, 1, &dnsTaskHandle);

  // Set up the access point
  setupWifiAP();

  dnsServer.start(DNS_PORT, "akowe.org", WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlForm);
  });

  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request) {
    String input1, input2;
    if (request->hasParam("input1")) {
      input1 = request->getParam("input1")->value();
    }
    if (request->hasParam("input2")) {
      input2 = request->getParam("input2")->value();
    }
    String response = "Input 1: " + input1 + "<br>Input 2: " + input2;
    request->send(200, "text/html", response);
  });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
