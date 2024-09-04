#ifndef NETWORK_H
#define NETWORK_H
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <str_num_msg_transcode.h>

#define NUM_BUFFER_SIZE 16

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern DNSServer dnsServer;
extern IPAddress local_IP;
/* extern const char* domainName; */

#ifdef FAKE_VOLTAGE_READING
extern bool test_auto_mode;
#endif

void send_binary_data(void *data, size_t len);
void send_num_message_to_a_client(Num value, uint8_t type_id,
                                  AsyncWebSocketClient *client);
void send_num_message(Num &value, uint8_t type_id);

#endif // NETWORK_H