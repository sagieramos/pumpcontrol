#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"
#include <str_num_msg_transcode.h>

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

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

void send_num_message(Num &value, uint8_t type_id,
                      AsyncWebSocketClient *client) {
  uint8_t buffer[Num_size];
  size_t buffer_size = Num_size;
  if (serialize_num(value, buffer, &buffer_size, type_id)) {
    LOG_F("Sent Num message. key: %d, value: %f | Type ID: %d | "
          "Buffer size: %d\n",
          value.key, value.value, type_id, buffer_size);
    send_binary_data(client, buffer, buffer_size);
  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

void send_msg1_data(Msg1 &msg1, uint8_t type_id, AsyncWebSocketClient *client) {
  uint8_t buffer[Msg1_size];
  size_t buffer_size = Msg1_size;
  if (serialize_msg1(msg1, buffer, &buffer_size, type_id)) {
    LOG_F("Sent Msg1 message. f0: %f, f1: %f, f2: %f | Type ID: %d | "
          "Buffer size: %d\n",
          msg1.f0, msg1.f1, msg1.f2, type_id, buffer_size);
    send_binary_data(client, buffer, buffer_size);
  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  size_t client_id = client->id();

  if (type == WS_EVT_CONNECT) {
    ws.cleanupClients();
    LOG_F("Websocket client connection received: %u\n", client_id);

    uint8_t buff[pump_ControlData_size];
    size_t buff_size = pump_ControlData_size;

    if (serialize_control_data(current_pump_data, buff, &buff_size,
                               CONTROL_DATA_TYPE_ID)) {
      LOG_F("Sent control data. Mode: %lu ms, Running: %lu ms, Resting: %d\n",
            current_pump_data.mode, current_pump_data.time_range.running,
            current_pump_data.time_range.resting);
    } else {
      LOG_LN("Failed to serialize control data");
    }
    client->binary(buff, buff_size);

    send_min_voltage(client);

    sendCurrentMachineState(client);

    check_and_resume_task(sendVoltageTask, ws.count() > 0);

    send_pzem_data(client);

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
