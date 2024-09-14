#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"
#include <str_num_msg_transcode.h>

AsyncWebSocket ws("/ws");

void send_binary_data(void *data, size_t len) {
  uint8_t *buffer = static_cast<uint8_t *>(data);
  if (ws.count() > 0) {
    ws.binaryAll(buffer, len);
  }
}

void send_binary_data(AsyncWebSocketClient *client, uint8_t *buffer,
                      size_t buffer_size) {
  if (client == nullptr || client->status() != WS_CONNECTED) {
    if (ws.count() > 0) {
      ws.binaryAll(buffer, buffer_size);
    }
  } else {
    client->binary(buffer, buffer_size);
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  size_t client_id = client->id();

  if (type == WS_EVT_CONNECT) {
    ws.cleanupClients();
    LOG_F("Websocket client connection received: %u\n", client_id);

    uint8_t buff[56];
    size_t buff_size = sizeof(buff);

    if (serialize_control_data(current_pump_data, buff, &buff_size,
                               CONTROL_DATA_TYPE_ID)) {
      LOG_F("Sent control data. Mode: %lu ms, Running: %lu ms, Resting: %d\n",
            current_pump_data.mode, current_pump_data.time_range.running,
            current_pump_data.time_range.resting);
    } else {
      LOG_LN("Failed to serialize control data");
    }
    client->binary(buff, buff_size);

    // send min voltage
    Num msg = Num_init_zero;
    msg.key = VoltageKey::MIN_VOLTAGE;
    msg.value = min_voltage;
    send_num_message_to_a_client(msg, NUM_TYPE_ID, client);

    send_all_power_status_and_type(client);

    check_and_resume_task(sendVoltageTask, ws.count() > 0);

    send_pzem_data();

    LOG_F("Clients online: %d\n", ws.count());
  } else if (type == WS_EVT_DISCONNECT) {
    check_and_resume_task(sendVoltageTask, ws.count() > 0);
    LOG_F("Clients online: %d\n", ws.count());
  } else if (type == WS_EVT_ERROR) {
    LOG_LN("Websocket error");
  } else if (type == WS_EVT_PONG) {
  } else if (WS_EVT_DATA) {
    LOG_F("Websocket data received: %u\n", client_id);
    receive_msg_and_perform_action(data, len, data[0]);
  } else {
    LOG_LN("Websocket event not handled");
  }
}
