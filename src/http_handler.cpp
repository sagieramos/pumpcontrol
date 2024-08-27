#include "main.h"
#include "network.h"

AsyncWebServer server(80);

ClientSession authClients[MAX_CLIENTS];

void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType) {
  String userAgent = request->header("User-Agent");
  LOG_F("User-Agent: %s\n", userAgent.c_str());
  LOG_LN("..........................................");
  bool isMobile = userAgent.indexOf("Android") != -1 ||
                  userAgent.indexOf("iPhone") != -1 ||
                  userAgent.indexOf("iPad") != -1;

  String gzPath = String(path) + ".gz";

  if (!isMobile && SPIFFS.exists(gzPath)) {
    LOG_F("Serving gzipped file: %s\n", gzPath.c_str());
    AsyncWebServerResponse *response =
        request->beginResponse(SPIFFS, gzPath, contentType);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  } else if (SPIFFS.exists(path)) {
    LOG_F("Serving file: %s\n", path);
    request->send(SPIFFS, path, contentType);
  } else {
    request->send(404, "text/plain", "File Not Found");
    LOG_F("File not found: %s\n", path);
  }
}

void handleDashboard(AsyncWebServerRequest *request) {
  int auth = authSession(authClients, request, CHECK);
  if (auth == ACTIVE) {
    request->send(SPIFFS, "/_dashboard.html", "text/html");
    LOG_LN("Dashboard");
  } else {
    request->redirect("/");
    LOG_LN("Unauthorized");
  }
}

void handleLogout(AsyncWebServerRequest *request) {
  int auth = authSession(authClients, request, LOGOUT);
  if (auth == DEAUTHENTICATED) {
    request->redirect("/");
    LOG_LN("Logged out");
  } else {
    request->redirect("/");
    LOG_LN("Unauthorized");
  }
}

void handleLogin(AsyncWebServerRequest *request) {
  if (request->hasParam("pin", true)) {
    ClientSession session;
    AuthStatus auth = authSession(authClients, request, session, LOGIN);

    LOG_F("auth value: %d\n", auth);

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

      LOG_LN("Login successful");
      break;
    }
    case SESSION_IS_FULL:
      request->send(500, "text/plain", "Session is full");
      LOG_LN("Session is full");
      break;
    case UNAUTHORIZED:
    default:
      request->send(401, "text/plain", "PIN is incorrect or unauthorized");
      LOG_LN("PIN is incorrect or unauthorized");
      break;
    }
  } else {
    request->send(400, "text/html", "No PIN provided");
    LOG_LN("No PIN provided");
  }
}

void handleRequest(AsyncWebServerRequest *request) {
  char urlPath[100];
  int method = request->method();

  // Get the URL path
  strcpy(urlPath, request->url().c_str());

  // Debug printouts for request details
  LOG_F("Request from: %s:%d\n",
        request->client()->remoteIP().toString().c_str(),
        request->client()->remotePort());
  LOG_F("http://%s => Method: %d\n", urlPath, method);

  if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
    // Handle root route (example)
    int auth = authSession(authClients, request, CHECK);
    if (auth == ACTIVE) {
      request->redirect("/dashboard");
      LOG_LN("Already logged in");
    } else {
      request->send(SPIFFS, "/_login.html", "text/html");
      LOG_LN("Serving _login.html");
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
      LOG_LN("Dashboard");
    } else {
      request->redirect("/");
    }
  } else {
    // Handle other routes (404 Not Found)
    request->send(404, "text/plain", "Not found");
    LOG_LN("Not found");
  }
}