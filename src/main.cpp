#include "main.h"
#include <ESPAsyncWebServer.h>

void setup() {
  DEBUG_SERIAL_BEGIN(115200);
  // Serial.begin(115200);
  // delete old config

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

  server.on("*", HTTP_ANY, [](AsyncWebServerRequest *request) {
    char urlPath[100];
    request->url().toCharArray(urlPath, sizeof(urlPath));
    int method = request->method();
    // get the request info IP
    DEBUG_SERIAL_PRINTF("Request from: %s:%d\n",
                        request->client()->remoteIP().toString().c_str(),
                        request->client()->remotePort());
    DEBUG_SERIAL_PRINTF("%s => Method: %d\n", urlPath, method);

    if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
      ClientSession *session =
          getSessionFromRequest(request, authenticatedClients);
      if (session && isClientSessionActive(session)) {
        request->send(200, "text/plain", "Already logged in");
        DEBUG_SERIAL_PRINTLN("Already logged in");
        return;
      }

      // If not authenticated, send the login form
      DEBUG_SERIAL_PRINTLN("Sending login form");
      request->send(200, "text/html",
                    "<form method='POST' action='/login'>"
                    "<input type='password' name='pin' placeholder='PIN'>"
                    "</br>"
                    "<input type='submit' value='Login'>"
                    "</form>");
      return;
    } else if (strcmp(urlPath, "/login") == 0 && method == HTTP_POST) {
      if (request->hasParam("pin", true)) {
        String pin = request->getParam("pin", true)->value();
        if (pin == PIN) { // Replace this with your actual PIN checking logic
          // Find an empty session slot
          DEBUG_SERIAL_PRINTLN("Finding an available session slot");
          for (int i = 0; i < MAX_CLIENTS; i++) {
            if (authenticatedClients[i].token[0] == '\0' ||
                isClientSessionActive(&authenticatedClients[i])) {
              generateSessionToken(authenticatedClients[i].token, TOKEN_LENGTH);
              updateClientSession(&authenticatedClients[i]);
              DEBUG_SERIAL_PRINTLN("Session updated");

              // Send the token to the client as a cookie
              String cookie =
                  "_imuwahen=" + String(authenticatedClients[i].token) +
                  "; Path=/" + "; Max-Age=3600";
              +"; Secure";
              +"; HttpOnly";
              +"; SameSite=Strict";
              +"; Domain=akowe.org";
              AsyncWebServerResponse *response =
                  request->beginResponse(200, "text/plain", "Login successful");
              response->addHeader("Set-Cookie", cookie);
              request->send(response);
              DEBUG_SERIAL_PRINTLN("Login successful");
              return;
            }
          }
          request->send(500, "text/plain", "Session is full");
          DEBUG_SERIAL_PRINTLN("Session is full");
        } else {
          request->send(401, "text/plain", "Invalid PIN");
          DEBUG_SERIAL_PRINTLN("Invalid PIN");
          return;
        }
      } else {
        request->send(400, "text/plain", "No PIN provided");
        DEBUG_SERIAL_PRINTLN("No PIN provided");
        return;
      }
    }

    else {
      request->send(404, "text/plain", "Not found");
      DEBUG_SERIAL_PRINTLN("Not found");
    }
  });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
