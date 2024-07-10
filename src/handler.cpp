#include "main.h"

ClientSession authClients[MAX_CLIENTS];

void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType) {
  if (!SPIFFS.exists(path)) {
    request->send(404, "text/plain", "File Not Found");
    DEBUG_SERIAL_PRINTF("File not found: %s\n", path);
    return;
  }

  AsyncWebServerResponse *response =
      request->beginResponse(SPIFFS, path, contentType);
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response->addHeader("Access-Control-Allow-Headers",
                      "Content-Type, Authorization, X-Requested-With");
  request->send(response);
}

void handleDashboad(AsyncWebServerRequest *request) {
  int auth = authSession(authClients, request, CHECK);
  if (auth == ACTIVE) {
    request->send(SPIFFS, "/_dashboard.html", "text/html");
    DEBUG_SERIAL_PRINTLN("Dashboard");
  } else {
    request->redirect("/");
    DEBUG_SERIAL_PRINTLN("Unauthorized");
  }
}

void handleLogout(AsyncWebServerRequest *request) {
  int auth = authSession(authClients, request, LOGOUT);
  if (auth == DEAUTHENTICATED) {
    request->redirect("/");
    DEBUG_SERIAL_PRINTLN("Logged out");
  } else {
    request->redirect("/");
    DEBUG_SERIAL_PRINTLN("Unauthorized");
  }
}

void handleLogin(AsyncWebServerRequest *request) {
  if (request->hasParam("pin", true)) {
    String pin = request->getParam("pin", true)->value();
    ClientSession session;
    AuthStatus auth = authSession(authClients, request, session, LOGIN);

    DEBUG_SERIAL_PRINTF("auth value: %d\n", auth);

    switch (auth) {
    case AUTHENTICATED: {
      // Generate cookies and send response
      const String cookieOptions =
          "; Path=/; Max-Age=3600; HttpOnly; SameSite=Strict; Domain=akowe.org";
      String cookie1 =
          String(TOKEN_ATTR) + "=" + String(session.token) + cookieOptions;
      String cookie2 =
          String(INDEX_ATTR) + "=" + String(session.index) + cookieOptions;

      AsyncWebServerResponse *response =
          request->beginResponse(SPIFFS, "/_dashboard.html", "text/html");
      response->addHeader("Set-Cookie", cookie1);
      response->addHeader("Set-Cookie", cookie2);
      request->send(response);

      DEBUG_SERIAL_PRINTLN("Login successful");
      break;
    }
    case SESSION_IS_FULL:
      request->send(500, "text/plain", "Session is full");
      DEBUG_SERIAL_PRINTLN("Session is full");
      break;
    case UNAUTHORIZED:
    default:
      request->send(401, "text/plain", "PIN is incorrect or unauthorized");
      DEBUG_SERIAL_PRINTLN("PIN is incorrect or unauthorized");
      break;
    }
  } else {
    request->send(400, "text/html", "No PIN provided");
    DEBUG_SERIAL_PRINTLN("No PIN provided");
  }
}

void handleRequest(AsyncWebServerRequest *request) {
  char urlPath[100];
  int method = request->method();

  // Get the URL path
  strcpy(urlPath, request->url().c_str());

  // Debug printouts for request details
  DEBUG_SERIAL_PRINTF("Request from: %s:%d\n",
                      request->client()->remoteIP().toString().c_str(),
                      request->client()->remotePort());
  DEBUG_SERIAL_PRINTF("http://%s => Method: %d\n", urlPath, method);

  if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
    // Handle root route (example)
    int auth = authSession(authClients, request, CHECK);
    if (auth == ACTIVE) {
      request->redirect("/dashboard");
      DEBUG_SERIAL_PRINTLN("Already logged in");
    } else {
      request->send(SPIFFS, "/_lindex.html", "text/html");
      DEBUG_SERIAL_PRINTLN("Serving _lindex.html");
    }
  } else if (strcmp(urlPath, "/login") == 0 && method == HTTP_POST) {
    // Handle login route
    handleLogin(request);
  } else if (strcmp(urlPath, "/logout") == 0 && method == HTTP_GET) {
    // Handle logout route
    handleLogout(request);
  } else if (strcmp(urlPath, "/dashboard") == 0 && method == HTTP_GET) {
    // Handle dashboard route
    int auth = authSession(authClients, request, CHECK);
    if (auth == ACTIVE) {
      request->send(SPIFFS, "/_dashboard.html", "text/html");
      DEBUG_SERIAL_PRINTLN("Dashboard");
    } else {
      request->redirect("/");
    }
  } else {
    // Handle other routes (404 Not Found)
    request->send(404, "text/plain", "Not found");
    DEBUG_SERIAL_PRINTLN("Not found");
  }
}