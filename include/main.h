#ifndef MAIN_H
#define MAIN_H

#include "FS.h"
#include "dev_or_prod.h"
#include "session.h"
#include <SPIFFS.h>
#include <WiFi.h>

extern AsyncWebServer server;

#define LED_PIN 2

#define MAX_BUFFER_SIZE 1024

constexpr uint8_t DNS_PORT = 53;

void setupWifiAP();

void taskBlink(void *pvParameters);
void dnsTask(void *pvParameters);

extern const byte DNS_PORT;
extern uint8_t numStations;

void serveStaticFile(AsyncWebServerRequest *request, const char *path,
                     const char *contentType);
void handleRequest(AsyncWebServerRequest *request);
ClientSession *findClientSessionByIndex(ClientSession *authClients,
                                        size_t index);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len);
void stackMonitor(void *pvParameter);

#endif // MY_HEADER_H
