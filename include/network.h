#ifndef NETWORK_H
#define NETWORK_H
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern DNSServer dnsServer;

#endif // NETWORK_H