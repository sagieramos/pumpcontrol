#ifndef MAIN_H
#define MAIN_H

#include "FS.h"
#include "control.h"
#include "server.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WiFi.h>

#define PIN "1234"
#define LED_PIN 2

constexpr uint8_t DNS_PORT = 53; // DNS server port
constexpr uint8_t MAX_CLIENTS =
    4; // Maximum number of clients that can be AUTHENTICATED
constexpr size_t TOKEN_LENGTH = 9;        // 8 characters + null terminator
constexpr size_t SESSION_TIMEOUT = 60000; // 1 minute
constexpr const char *TOKEN_ATTR = "_imuwahen";
constexpr const char *INDEX_ATTR = "_idx";

// EEPROM address for control data
constexpr uint8_t EEPROM_SIZE_CTL = sizeof(uint8_t) + sizeof(control);

#define DEBUG_SERIAL_ENABLED // Comment or uncomment this line to toggle
// serial output

#ifdef DEBUG_SERIAL_ENABLED
#define DEBUG_SERIAL_BEGIN(baud) Serial.begin(baud)
#define DEBUG_SERIAL_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_SERIAL_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_SERIAL_BEGIN(baud) ((void)0)
#define DEBUG_SERIAL_PRINT(...) ((void)0)
#define DEBUG_SERIAL_PRINTLN(...) ((void)0)
#define DEBUG_SERIAL_PRINTF(...) ((void)0)
#endif

// Function prototypes
void setupWifiAP();
// void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Function prototype for blink task
void taskBlink(void *pvParameters);
void dnsTask(void *pvParameters);
void taskRun(void *pvParameters);
// void stackMonitor(void *pvParameter);

struct ClientSession {
  char token[TOKEN_LENGTH]; // Session token
  unsigned long startTime;  // Start time of the session
  unsigned long lastActive; // Last ACTIVE time of the session
  unsigned int index;       // Index of the session in the session array
  uint32_t clientId;

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
    return strcmp(token, session.token) == 0;
  }

  bool operator!=(const ClientSession &session) const {
    return strcmp(token, session.token) != 0;
  }

  /*   bool operator==(const char *sessionToken) const {
      return strcmp(token, sessionToken) == 0;
    }

    bool operator!=(const char *sessionToken) const {
      return strcmp(token, sessionToken) != 0;
    }

    bool operator==(const uint32_t id) const { return clientId == id; }

    bool operator!=(const uint32_t id) const { return clientId != id; } */
};

// Task handle for the blink task
extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t dnsTaskHandle;
extern TaskHandle_t runMachineTask;
extern const byte DNS_PORT;
extern DNSServer dnsServer;
extern ClientSession authClients[MAX_CLIENTS];
extern AsyncWebSocket ws;
// extern AsyncWebServer server;
// extern ClientSession authClients[MAX_CLIENTS];

String getTaskInfo(TaskHandle_t taskHandle);

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
String serializeMachineData(MachineMode mode, unsigned int running,
                            unsigned int resting,
                            const char *objectName = "machineConfig");
bool deserializeMachineData(const char *jsonString, controlData &machineData,
                            const char *objectName = "machineConfig");

uint32_t getCurrentTimeMs();
// Constants
/* const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *mqtt_server = "mqtt.example.com";
const int mqtt_port = 1883; */

// Macros

#endif // MY_HEADER_H
