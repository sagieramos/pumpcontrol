#include "main.h"
#include "network.h"

const char *domainName = "akowe.org";

#ifdef FAKE_VOLTAGE_READING
bool test_auto_mode = false;
#endif

#ifndef PRODUCTION
const char *http_methods[] = {"", "GET", "POST"};
#endif

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
      // Common cookie options
      const String commonCookieOptions =
          "; Path=/; Max-Age=3600; HttpOnly; SameSite=Strict";

      String host = request->host();
      LOG_F("Host name.....%s\n", host.c_str());
      String cookieOptions;

      if (host.indexOf('.') != -1) {
        cookieOptions = commonCookieOptions + "; Domain=" + host;
      } else {
        cookieOptions = commonCookieOptions;
      }

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
  uint8_t allowedMethods =
      WebRequestMethod::HTTP_GET | WebRequestMethod::HTTP_POST;
  uint8_t method = request->method();

  if (!(method & allowedMethods)) {
    request->send(405);
    LOG_LN("Method Not Allowed. Only GET and POST");
    return;
  }

  const char *host = request->host().c_str();
  const char *localIPCStr = local_IP.toString().c_str();

  if (strcmp(host, domainName) != 0 && strcmp(host, localIPCStr) != 0) {
    static const char *jsContentPrefix =
        "<script>window.location.href = 'http://";
    static const char *jsContentSuffix = "';</script>";

    char jsContent[128];
    snprintf(jsContent, sizeof(jsContent), "%s%s%s", jsContentPrefix,
             domainName, jsContentSuffix);
    request->send(200, "text/html", jsContent);
    return;
  }

  char urlPath[100];

  // Get the URL path
  strcpy(urlPath, request->url().c_str());

  // Debug printouts for request details
  LOG_F("Request from: %s:%d\n",
        request->client()->remoteIP().toString().c_str(),
        request->client()->remotePort());
  LOG_F("%s\thttp://%s%s\n", http_methods[method], request->host().c_str(),
        urlPath);

  if (strcmp(urlPath, "/") == 0 && method == HTTP_GET) {
    // Handle root route (example)
    int auth = authSession(authClients, request, CHECK);
    if (auth == ACTIVE) {
      request->redirect("/dashboard");
      LOG_LN("Already logged in");
    } else {
      request->send(SPIFFS, "/_login.html", "text/html");
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
  }
#ifdef FAKE_VOLTAGE_READING
  else if (strcmp(urlPath, "/readtest") == 0 && method == HTTP_GET) {
    // Handle test route
    test_auto_mode = !test_auto_mode;
    request->send(200, "text/plain", test_auto_mode ? "true" : "false");
    LOG_F("Test auto mode: %s\n", test_auto_mode ? "true" : "false");
  }
#endif
  else {
    // Handle other routes (404 Not Found)
    request->send(404, "text/plain", "Not found");
    LOG_LN("Not found");
  }
}