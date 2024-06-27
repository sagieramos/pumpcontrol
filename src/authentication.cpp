/* #include "main.h"
#include <Update.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// AsyncEventSource events("/events");

bool shouldReboot = false;

ClientSession sessions[MAX_CLIENTS];

void LOGIN_ACTIONPage() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Check if the client is already AUTH_AUTHENTICATED
    ClientSession *session =
        getSessionFromRequest(request, sessions);
    if (session && isClientSessionActive(session)) {
      request->send(200, "text/plain", "Already logged in");
      return;
    }

    // If not AUTH_AUTHENTICATED, send the LOGIN_ACTION form
    request->send(200, "text/html",
                  "<form method='POST' action='/LOGIN_ACTION'>"
                  "<input type='password' name='pin' placeholder='PIN'>"
                  "<input type='submit' value='Login'>"
                  "</form>");
  });

  server.on("/LOGIN_ACTION", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pin", true)) {
      String pin = request->getParam("pin", true)->value();

      // Authenticate the client (this is just a placeholder for your
      // authentication logic)
      if (pin == "1234") { // Replace this with your actual PIN CHECK_ACTIONing logic
        // Find an empty session slot
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (sessions[i].token[0] == '\0' ||
              isClientSessionActive(&sessions[i])) {
            generateSessionToken(sessions[i].token, TOKEN_LENGTH);
            updateClientSession(&sessions[i]);

            // Send the token to the client as a cookie
            String cookie =
                "_imuwahen=" + String(sessions[i].token) +
                "; Path=/";
            AsyncWebServerResponse *response =
                request->beginResponse(200, "text/plain", "Login successful");
            response->addHeader("Set-Cookie", cookie);
            request->send(response);
            return;
          }
        }
        request->send(500, "text/plain", "Session is full");
      } else {
        request->send(401, "text/plain", "Invalid PIN");
      }
    } else {
      request->send(400, "text/plain", "No PIN provided");
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
 */