#include "main.h"

ClientSession authenticatedClients[MAX_CLIENTS];

void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType) {
  if (!SPIFFS.exists(path)) {
    request->send(404, "text/plain", "File Not Found");
    DEBUG_SERIAL_PRINTF("File not found: %s\n", path);
    return;
  }

  DEBUG_SERIAL_PRINTF("Serving file: %s\n", path);

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
    int auth = authSession(authenticatedClients, request, CHECK);
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
      int auth = authSession(authenticatedClients, request, session, LOGIN);
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
      request->send(401, "text/plain",
                    "The PIN that you've entered is incorrect.");
      DEBUG_SERIAL_PRINTLN("Invalid PIN format");
    }
  } else {
    request->send(400, "text/html", "No PIN provided");
    DEBUG_SERIAL_PRINTLN("No PIN provided");
  }
}

void handleLogout(AsyncWebServerRequest *request) {
  int auth = authSession(authenticatedClients, request, LOGOUT);
  if (auth == deauthenticated) {
    request->redirect("/");
    DEBUG_SERIAL_PRINTLN("Logged out");
  } else {
    request->send(401, "text/plain", "Unauthorized");
    DEBUG_SERIAL_PRINTLN("Unauthorized");
  }
}
