#include "main.h"
#include "FS.h"
#include <SPIFFS.h>

void handleLogin(AsyncWebServerRequest *request);
void handleLogout(AsyncWebServerRequest *request);
void serveStaticFile(AsyncWebServerRequest *request, String path,
                     String contentType);
void handleRequest(AsyncWebServerRequest *request);

ClientSession authenticatedClients[MAX_CLIENTS];
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

  // Setup WiFi AP and DNS
  setupWifiAP();
  dnsServer.start(DNS_PORT, "akowe.org", WiFi.softAPIP());

  // Serve static files with appropriate MIME types and CORS headers
  server.on("/_lbundle.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/_lbundle.js", "application/javascript");
  });

  server.on("/_lindex.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/_lindex.js", "application/javascript");
  });

  server.on("/_lstyles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/_lstyles.css", "text/css");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/favicon.ico", "image/x-icon");
  });

  server.on("/logo.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/logo.svg", "image/svg+xml");
  });

  server.on("/warning.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
    serveStaticFile(request, "/warning.svg", "image/svg+xml");
  });

  // Handle other requests including login, logout, and default routes
  server.on("*", HTTP_ANY,
            [](AsyncWebServerRequest *request) { handleRequest(request); });

  server.begin();
}

void loop() { vTaskDelay(portMAX_DELAY); }

void serveStaticFile(AsyncWebServerRequest *request, String path,
                     String contentType) {
  AsyncWebServerResponse *response =
      request->beginResponse(SPIFFS, path, contentType);
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response->addHeader("Access-Control-Allow-Headers",
                      "Content-Type, Authorization, X-Requested-With");
  request->send(response);
}

void handleRequest(AsyncWebServerRequest *request) {
  char urlPath[100];
  request->url().toCharArray(urlPath, sizeof(urlPath));
  int method = request->method();

  // Debug printouts for request details
  DEBUG_SERIAL_PRINTF("Request from: %s:%d\n",
                      request->client()->remoteIP().toString().c_str(),
                      request->client()->remotePort());
  DEBUG_SERIAL_PRINTF("%s => Method: %d\n", urlPath, method);

  if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
    // Handle root route (example)
    int auth = authSession(authenticatedClients, request, check);
    if (auth == active) {
      request->send(200, "text/plain", "Already logged in");
      DEBUG_SERIAL_PRINTLN("Already logged in");
    } else {
      request->send(SPIFFS, "/_lindex.html", "text/html");
    }
  } else if (strcmp(urlPath, "/login") == 0 && method == HTTP_POST) {
    // Handle login route
    handleLogin(request);
  } else if (strcmp(urlPath, "/logout") == 0 && method == HTTP_GET) {
    // Handle logout route
    handleLogout(request);
  } else {
    // Handle other routes (404 Not Found)
    request->send(404, "text/plain", "Not found");
    DEBUG_SERIAL_PRINTLN("Not found");
  }
}

void handleLogin(AsyncWebServerRequest *request) {
  if (request->hasParam("pin", true)) {
    String pin = request->getParam("pin", true)->value();
    if (pin == PIN) {
      // Handle successful login
      ClientSession session;
      int auth = authSession(authenticatedClients, request, login, session);
      if (auth == authenticated) {
        // Generate cookies and send response
        String cookie1 = "_imuwahen=" + String(session.token) +
                         "; Path=/; Max-Age=3600; HttpOnly; SameSite=Strict; "
                         "Domain=akowe.org";
        String cookie2 = "_index=" + String(session.index) +
                         "; Path=/; Max-Age=3600; HttpOnly; SameSite=Strict; "
                         "Domain=akowe.org";
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
        request->send(401, "text/plain", "PIN is incorrect");
        DEBUG_SERIAL_PRINTLN("PIN is incorrect");
      }
    } else {
      request->send(401, "text/plain", "You provided a wrong PIN");
      DEBUG_SERIAL_PRINTLN("Invalid PIN format. Must be 4 digits long");
    }
  } else {
    request->send(400, "text/html", "No PIN provided");
    DEBUG_SERIAL_PRINTLN("No PIN provided");
  }
}

void handleLogout(AsyncWebServerRequest *request) {
  int auth = authSession(authenticatedClients, request, logout);
  if (auth == deauthenticated) {
    request->redirect("/");
    DEBUG_SERIAL_PRINTLN("Logged out");
  } else {
    request->send(401, "text/plain", "Unauthorized");
    DEBUG_SERIAL_PRINTLN("Unauthorized");
  }
}
