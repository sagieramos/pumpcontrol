#ifndef MAIN_H
#define MAIN_H

#include "FS.h"
#include "control.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

#define PIN "1234"
#define LED_PIN 2

constexpr uint8_t DNS_PORT = 53; // DNS server port
constexpr uint8_t MAX_CLIENTS =
    4; // Maximum number of clients that can be authenticated
constexpr size_t TOKEN_LENGTH = 9;        // 8 characters + null terminator
constexpr size_t SESSION_TIMEOUT = 60000; // 1 minute

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

struct ClientSession {
  char token[TOKEN_LENGTH]; // Session token
  unsigned long startTime;  // Start time of the session
  unsigned long lastActive; // Last active time of the session
  unsigned int index;       // Index of the session in the session array
};

// Task handle for the blink task
extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t dnsTaskHandle;
extern const byte DNS_PORT;
extern DNSServer dnsServer;
// extern AsyncWebServer server;
// extern ClientSession authenticatedClients[MAX_CLIENTS];

String getTaskInfo(TaskHandle_t taskHandle);

void generateSessionToken(char *buffer, size_t length);

enum AuthStatus {
  active,
  notActive,
  authenticated,
  sessionIsFull,
  noTokenProvided,
  unauthorized,
  deauthenticated
};

enum authAction { login, logout, check };

ClientSession *getSessionFromRequest(AsyncWebServerRequest *request,
                                     ClientSession *authenticatedClients);
ClientSession *findClientSession(ClientSession *authenticatedClients,
                                 const char *token);
ClientSession *findClientSession(ClientSession *authenticatedClients,
                                 int index);
AuthStatus authSession(ClientSession *authenticatedClients,
                       AsyncWebServerRequest *request, authAction action,
                       ClientSession &session);
AuthStatus authSession(ClientSession *authenticatedClients,
                       AsyncWebServerRequest *request, authAction action);

void handleLogin(AsyncWebServerRequest *request);
void handleLogout(AsyncWebServerRequest *request);
void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType);
void handleRequest(AsyncWebServerRequest *request);

// Constants
/* const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *mqtt_server = "mqtt.example.com";
const int mqtt_port = 1883; */

// Macros

#endif // MY_HEADER_H
