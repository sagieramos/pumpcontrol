#ifndef NETWORK_H
#define NETWORK_H
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <str_num_msg_transcode.h>
#include <transcode_msg1.h>
#include <transcode_pump_control.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncEventSource events;
extern DNSServer dnsServer;
extern IPAddress local_IP;
extern TaskHandle_t msgGatewayTaskHandle;
/* extern const char* domainName; */

#ifdef FAKE_VOLTAGE_READING
extern bool test_auto_mode;
#endif

void clearEEPROM();

void send_min_voltage(AsyncWebSocketClient *client = nullptr);
void send_pzem_data(AsyncWebSocketClient *client = nullptr);
void sendCurrentMachineState(AsyncWebSocketClient *client = nullptr);

void send_binary_data(void *data, size_t len);
void send_binary_data(AsyncWebSocketClient *client, uint8_t *buffer,
                      size_t buffer_size);

void send_num_message(Num &value, uint8_t type_id,
                      AsyncWebSocketClient *client = nullptr);
void send_msg1_data(Msg1 &msg1, uint8_t type_id,
                    AsyncWebSocketClient *client = nullptr);

#endif // NETWORK_H