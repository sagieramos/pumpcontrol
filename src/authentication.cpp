#include "main.h"
#include <Update.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

bool shouldReboot = false;

ClientSession authenticatedClients[MAX_CLIENTS];

void login() {
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {
    ClientSession *session = getSessionFromRequest(request);
    if (session) {
      session->lastActive = millis(); // Update last active time
      request->send(200, "text/plain", "Already logged in");
      return;
    }

    if (request->hasParam("pin", true)) {
      AsyncWebParameter *pin = request->getParam("pin", true);
      if (strcmp(pin->value().c_str(), "0000") == 0) {
        char token[TOKEN_LENGTH];

        generateSessionToken(token, TOKEN_LENGTH);

        if (findClientSession(token) == NULL) {
          // Store the new client session
          unsigned long currentTime = millis();
          for (int i = 0; i < MAX_CLIENTS + 1; i++) {
            if (MAX_CLIENTS == i) {
              request->send(500, "text/plain",
                            "Server Error: Maximum clients reached");
              return;
            }
            if (authenticatedClients[i].token[0] ==
                '\0') { // Find an empty slot
              strncpy(authenticatedClients[i].token, token, TOKEN_LENGTH);
              authenticatedClients[i].startTime = currentTime;
              authenticatedClients[i].lastActive = currentTime;
              break;
            }
          }

          // Set token as a cookie
          AsyncWebServerResponse *response =
              request->beginResponse(200, "text/plain", token);
          response->addHeader("Set-Cookie", String("imuwahen=") + token);
          request->send(response);
        } else {
          request->send(500, "text/plain",
                        "Server Error: Unable to generate unique token");
        }
      } else {
        request->send(401, "text/plain", "Unauthorized");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });
}

void update() {
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html",
                  "<form method='POST' action='/update' "
                  "enctype='multipart/form-data'><input type='file' "
                  "name='update'><input type='submit' value='Update'></form>");
  });

  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        shouldReboot = !Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(
            200, "text/plain", shouldReboot ? "OK" : "FAIL");
        response->addHeader("Connection", "close");
        request->send(response);
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        if (!index) {
          DEBUG_SERIAL_PRINTF("Update Start: %s\n", filename.c_str());
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
          }
        }
        if (!Update.hasError()) {
          if (Update.write(data, len) != len) {
            Update.printError(Serial);
          }
        }
        if (final) {
          if (Update.end(true)) {
            DEBUG_SERIAL_PRINTF("Update Success: %uB\n", index + len);
          } else {
            Update.printError(Serial);
          }
        }
      });
}
