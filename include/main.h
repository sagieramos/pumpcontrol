#ifndef MAIN_H
#define MAIN_H

#include "FS.h"
#include "dev_or_prod.h"
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

extern AsyncWebServer server;

#define PIN "1234"
#define LED_PIN 2

#define MAX_BUFFER_SIZE 1024

constexpr uint8_t DNS_PORT = 53; // DNS server port
constexpr uint8_t MAX_CLIENTS =
    8; // Maximum number of clients that can be AUTHENTICATED
constexpr size_t TOKEN_LENGTH = 9;        // 8 characters + null terminator
constexpr size_t SESSION_TIMEOUT = 60000; // 1 minute
constexpr const char *TOKEN_ATTR = "_imuwahen";
constexpr const char *INDEX_ATTR = "_idx";

// Function prototypes
void setupWifiAP();
// void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Function prototype for blink task
void taskBlink(void *pvParameters);
void dnsTask(void *pvParameters);
// void stackMonitor(void *pvParameter);

struct ClientSession {
  char token[TOKEN_LENGTH]; // Session token
  unsigned long startTime;  // Start time of the session
  unsigned long lastActive; // Last ACTIVE time of the session
  unsigned int index;       // Index of the session in the session array
  uint32_t clientId; // Client ID of the session in the WebSocket client array

  ClientSession() {
    memset(token, 0, TOKEN_LENGTH);
    startTime = 0;
    lastActive = 0;
    index = 0;
    clientId = 0;
  }

  ClientSession(const ClientSession &session) {
    strncpy(token, session.token, TOKEN_LENGTH);
    startTime = session.startTime;
    lastActive = session.lastActive;
    index = session.index;
    clientId = session.clientId;
  }

  ClientSession &operator=(const ClientSession &session) {
    if (this == &session) {
      return *this;
    }
    strncpy(token, session.token, TOKEN_LENGTH);
    startTime = session.startTime;
    lastActive = session.lastActive;
    index = session.index;
    clientId = session.clientId;
    return *this;
  }

  bool operator==(const ClientSession &session) const {
    return strcmp(token, session.token) == 0 &&
           startTime == session.startTime && lastActive == session.lastActive &&
           index == session.index && clientId == session.clientId;
  }

  bool operator!=(const ClientSession &session) const {
    return !(*this == session);
  }

  // Method to check if the session is null or uninitialized
  bool isNull() const {
    return token[0] == '\0' && startTime == 0 && lastActive == 0 &&
           index == 0 && clientId == 0;
  }

  // Boolean conversion operator to check if the session is null
  explicit operator bool() const { return !isNull(); }
};

// Task handle for the blink task
extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t dnsTaskHandle;
extern const byte DNS_PORT;
extern ClientSession authClients[MAX_CLIENTS];

enum AuthStatus {
  NOT_ACTIVE,
  ACTIVE,
  AUTHENTICATED,
  SESSION_IS_FULL,
  NO_TOKEN_PROVIDED,
  UNAUTHORIZED,
  DEAUTHENTICATED
};

enum authAction { LOGIN, LOGOUT, CHECK };

ClientSession *getSessionFromRequest(const String &cookieHeader,
                                     ClientSession *authClients);

AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, ClientSession &session,
                       authAction action);
AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, authAction action);

void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType);
void handleRequest(AsyncWebServerRequest *request);
ClientSession *findClientSessionByIndex(ClientSession *authClients,
                                        size_t index);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len);
void stackMonitor(void *pvParameter);

// Constants
/* const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *mqtt_server = "mqtt.example.com";
const int mqtt_port = 1883; */

// Macros

#endif // MY_HEADER_H
