#ifndef NETWORK_H
#define NETWORK_H
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <str_num_msg_transcode.h>
#include <transcode_msg1.h>
#include <transcode_pump_control.h>

enum MessageCommand : uint8_t {
  NUM = 1,
  CONTROL_DATA,
  MSG1,
};

struct MessageData {
  uint8_t data_type;
  uint8_t type_id;
  AsyncWebSocketClient *client;

  union {
    pump_ControlData *control_data;
    Num *num;
    Msg1 *msg1;
  } data;

  // Default constructor
  MessageData() : data_type(0), type_id(0), client(nullptr) {
    data.control_data = nullptr;
  }

  // Constructor for control_data
  MessageData(pump_ControlData *control_data_ref, uint8_t id,
              AsyncWebSocketClient *client_ref = nullptr)
      : data_type(MessageCommand::CONTROL_DATA), type_id(id),
        client(client_ref) {
    data.control_data = control_data_ref;
  }

  // Constructor for num
  MessageData(Num *num_ref, uint8_t id,
              AsyncWebSocketClient *client_ref = nullptr)
      : data_type(MessageCommand::NUM), type_id(id), client(client_ref) {
    data.num = num_ref;
  }

  // Constructor for msg1
  MessageData(Msg1 *msg1_ref, uint8_t id,
              AsyncWebSocketClient *client_ref = nullptr)
      : data_type(MessageCommand::MSG1), type_id(id), client(client_ref) {
    data.msg1 = msg1_ref;
  }

  ~MessageData() {
    if (data_type == MessageCommand::CONTROL_DATA && data.control_data) {
      delete data.control_data;
    } else if (data_type == MessageCommand::NUM && data.num) {
      delete data.num;
    } else if (data_type == MessageCommand::MSG1 && data.msg1) {
      delete data.msg1;
    }
  }
};

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern DNSServer dnsServer;
extern IPAddress local_IP;
extern TaskHandle_t msgGatewayTaskHandle;
/* extern const char* domainName; */

#ifdef FAKE_VOLTAGE_READING
extern bool test_auto_mode;
#endif

void clearEEPROM();

void send_binary_data(void *data, size_t len);
void send_binary_data(AsyncWebSocketClient *client, uint8_t *buffer,
                      size_t buffer_size);
void send_num_message_to_a_client(Num value, uint8_t type_id,
                                  AsyncWebSocketClient *client);
void send_num_message(Num &value, uint8_t type_id);

void msgGatewayTask(void *pvParameters);

#endif // NETWORK_H