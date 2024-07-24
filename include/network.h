#ifndef NETWORK_H
#define NETWORK_H
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern DNSServer dnsServer;

void send_binary_data(void *data, size_t len);

#endif // NETWORK_H