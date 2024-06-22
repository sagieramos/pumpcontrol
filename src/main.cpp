#include "main.h"
#include "FS.h"
#include <SPIFFS.h>

ClientSession authenticatedClients[MAX_CLIENTS];
AsyncWebServer server(80);


void setup() {
  DEBUG_SERIAL_BEGIN(115200);
  // Serial.begin(115200);
  // delete old config

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

/*   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  }); */

  server.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bundle.js", "application/javascript");
  });

    server.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.js", "application/javascript");
  });

  server.on("/styles_a.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles_a.css", "text/css");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.on("/logo.svg", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/logo.svg", "image/svg+xml");
  });

  server.on("*", HTTP_ANY, [](AsyncWebServerRequest *request) {
    char urlPath[100];
    request->url().toCharArray(urlPath, sizeof(urlPath));
    int method = request->method();

    // Get the request info IP
    DEBUG_SERIAL_PRINTF("Request from: %s:%d\n",
                        request->client()->remoteIP().toString().c_str(),
                        request->client()->remotePort());
    DEBUG_SERIAL_PRINTF("%s => Method: %d\n", urlPath, method);

    if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
      int auth = authSession(authenticatedClients, request, check);
      DEBUG_SERIAL_PRINTF("auth status: %d\n", auth);
      if (auth == active) {
        request->send(200, "text/plain", "Already logged in");
        DEBUG_SERIAL_PRINTLN("Already logged in");
      } else {
        // If not authenticated, send the login form
        DEBUG_SERIAL_PRINTLN("Sending login form");
        request->send(SPIFFS, "/index.html", "text/html");
      }
    } else if (strcmp(urlPath, "/login") == 0 && method == HTTP_POST) {
      if (request->hasParam("pin", true)) {
        String pin = request->getParam("pin", true)->value();
        if (pin == PIN) {
          ClientSession session;
          int auth = authSession(authenticatedClients, request, login, session);

          DEBUG_SERIAL_PRINTF("Login status: %d\n", auth);
          DEBUG_SERIAL_PRINTF("Session index: %d\n", session.index);
          DEBUG_SERIAL_PRINTF("Session token: %s\n", session.token);
          DEBUG_SERIAL_PRINTF("Session start time: %lu\n", session.startTime);
          DEBUG_SERIAL_PRINTF("Session last active: %lu\n", session.lastActive);
          DEBUG_SERIAL_PRINTF("Session token: %s\n", session.token);
          if (auth == authenticated) {
            String cookie1 = "_imuwahen=" + String(session.token) +
                             "; Path=/; Max-Age=3600; HttpOnly; "
                             "SameSite=Strict; Domain=akowe.org";

            String cookie2 = "_index=" + String(session.index) +
                             "; Path=/; Max-Age=3600; HttpOnly; "
                             "SameSite=Strict; Domain=akowe.org";

            AsyncWebServerResponse *response =
                request->beginResponse(200, "text/plain", "Login successful");
            response->addHeader("Set-Cookie", cookie1);
            response->addHeader("Set-Cookie", cookie2);
            request->send(response);

            DEBUG_SERIAL_PRINTLN("Login successful");
          } else if (auth == sessionIsFull) {
            request->send(500, "text/plain", "Session is full");
            DEBUG_SERIAL_PRINTLN("Session is full");
          } else {
            request->send(401, "text/plain", "Invalid PIN");
            DEBUG_SERIAL_PRINTLN("Invalid PIN");
          }
        } else {
          request->send(401, "text/plain", "Invalid PIN");
          DEBUG_SERIAL_PRINTLN("Invalid PIN");
        }
      } else {
        request->send(400, "text/html", "<h1>No PIN provided,</h1>"
                                         "<h2>Please provide a PIN</h2>"
                                         "<form method='POST' action='/login'>"
                                         "<input type='password' name='pin' "
                                         "placeholder='PIN'>"
                                         "<br>"
                                         "<input type='submit' value='Login'>"
                                         "</form>");
        DEBUG_SERIAL_PRINTLN("No PIN provided");
      }
    } else if (strcmp(urlPath, "/logout") == 0 && method == HTTP_GET) {
      int auth = authSession(authenticatedClients, request, logout);
      DEBUG_SERIAL_PRINTF("Logout status: %d\n", auth);
      if (auth == deauthenticated) {
        request->redirect("/");
        DEBUG_SERIAL_PRINTLN("Logged out");
      } else {
        request->send(401, "text/plain", "Unauthorized");
        DEBUG_SERIAL_PRINTLN("Unauthorized");
      }
    } else {
      request->send(404, "text/plain", "Not found");
      DEBUG_SERIAL_PRINTLN("Not found");
    }
  });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }
