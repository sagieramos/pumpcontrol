#include "main.h"
#include "network.h"
#include "session.h"

const char *ROOT_URL = "/";
const char *LOGIN_URL = "/login";
const char *LOGOUT_URL = "/logout";
const char *DASHBOARD_URL = "/dashboard";
const char *WIPE_CONFIG_URL = "/config/wipe";
const char *SESSION_ID_URL = "/getsessionid";
#ifdef FAKE_VOLTAGE_READING
const char *READ_TEST_URL = "/readtest";
#endif

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

  AsyncWebServerResponse *response;

  if (!isMobile && SPIFFS.exists(gzPath)) {
    LOG_F("Serving gzipped file: %s\n", gzPath.c_str());
    response = request->beginResponse(SPIFFS, gzPath, contentType);
    response->addHeader("Content-Encoding", "gzip");
  } else if (SPIFFS.exists(path)) {
    LOG_F("Serving file: %s\n", path);
    response = request->beginResponse(SPIFFS, path, contentType);
  } else {
    request->send(404, "text/plain", "File Not Found");
    LOG_F("File not found: %s\n", path);
    return;
  }

  if (String(contentType).startsWith("text/css") ||
      String(contentType).startsWith("application/javascript") ||
      String(contentType).startsWith("image/") ||
      String(contentType).startsWith("image/svg+xml") ||
      String(contentType).startsWith("image/x-icon")) {
    response->addHeader("Cache-Control", "max-age=31536000, public");
  } else if (String(contentType).startsWith("text/html")) {
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  }

  request->send(response);
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

static void sendJsonResponse(AsyncWebServerRequest *request, int statusCode,
                             const char *errorMessage) {
  pinGen.regenerate();

  char response[100];
  sprintf(response, "{\"error\": \"%s\", \"sessionId\": %lu}", errorMessage,
          pinGen.gen);
  request->send(statusCode, "application/json", response);

  events.send(String(pinGen.gen).c_str(), "sessionId", getCurrentTimeMs(),
              1000);
}

void handleLogin(AsyncWebServerRequest *request) {
  if (request->hasParam("pin", true)) {
    ClientSession session;
    AuthStatus auth = authSession(authClients, request, session, LOGIN);

    LOG_F("auth value: %d\n", auth);

    char res[100];

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

      pinGen.regenerate();

      events.send(String(pinGen.gen).c_str(), "sessionId", getCurrentTimeMs(),
                  1000);

      LOG_LN("Login successful");
      break;
    }

    case SESSION_IS_FULL:
      LOG_LN("Session is full");
      sendJsonResponse(request, 500, "Session is full");
      break;
    case UNAUTHORIZED:
      sendJsonResponse(request, 401, "PIN is incorrect");
      LOG_LN("PIN is incorrect");
      break;
    default:
      sendJsonResponse(request, 401, "unauthorized");
      LOG_LN("unauthorized");
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

  const String &urlPath = request->url();
  int auth = authSession(authClients, request, CHECK);

  // Debug printouts for request details
  LOG_F("Request from: %s:%d\n",
        request->client()->remoteIP().toString().c_str(),
        request->client()->remotePort());
  LOG_F("%s\thttp://%s%s\n", http_methods[method], request->host().c_str(),
        urlPath.c_str());

  if (urlPath == ROOT_URL && method == HTTP_GET) {
    if (auth == ACTIVE) {
      request->redirect("/dashboard");
      LOG_LN("Already logged in");
    } else {
      request->send(SPIFFS, "/_login.html", "text/html");
    }
  } else if (urlPath == LOGIN_URL && method == HTTP_POST) {
    handleLogin(request);
  } else if (urlPath == LOGOUT_URL && method == HTTP_GET) {
    handleLogout(request);
  } else if (urlPath == DASHBOARD_URL && method == HTTP_GET) {
    if (auth == ACTIVE) {
      request->send(SPIFFS, "/_dashboard.html", "text/html");
      LOG_LN("Dashboard");
    } else {
      request->redirect("/");
    }
  } else if (urlPath == WIPE_CONFIG_URL && method == HTTP_GET) {
    if (auth == ACTIVE) {
      clearEEPROM();
      request->send(
          200, "text/plain",
          "All stored data cleared successfully. Device restarting...");
      delay(1000);
      ESP.restart();
    } else {
      request->redirect("/");
    }
  } else if (urlPath == SESSION_ID_URL && method == HTTP_GET) {
    char sessionId[20];
    ultoa(pinGen.gen, sessionId, 10);
    request->send(200, "text/plain", sessionId);
  }
#ifdef FAKE_VOLTAGE_READING
  else if (urlPath == READ_TEST_URL && method == HTTP_GET) {
    test_auto_mode = !test_auto_mode;
    request->send(200, "text/plain", test_auto_mode ? "true" : "false");
    LOG_F("Test auto mode: %s\n", test_auto_mode ? "true" : "false");
  }
#endif
  else {
    request->send(404, "text/plain", "Not found");
    LOG_LN("Not found");
  }
}