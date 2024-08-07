#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "type_id.h"
#include <str_num_msg_transcode.h>

AsyncWebSocket ws("/ws");

void send_binary_data(void *data, size_t len) {
  uint8_t *buffer = static_cast<uint8_t *>(data);
  if (ws.count() > 0) {
    ws.binaryAll(buffer, len);
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  size_t client_id = client->id();

  if (type == WS_EVT_CONNECT) {
    ws.cleanupClients();
    DEBUG_SERIAL_PRINTF("Websocket client connection received: %u\n",
                        client_id);

    uint8_t buff[56];
    size_t buff_size = sizeof(buff);

    pump_ControlData ctr = get_current_control_data();

    serialize_control_data(ctr, buff, &buff_size, CONTROL_DATA_TYPE_ID);

    client->binary(buff, buff_size);

    // send min voltage
    Num msg = Num_init_zero;
    msg.key = VoltageKey::MIN_VOLTAGE;
   /*  msg.value = min_voltage; */
    msg.value = 188.0f;
    buff_size = sizeof(buff);
    if (serialize_num(msg, buff, &buff_size, VOLTAGE_TYPE_ID, NULL)) {
      client->binary(buff, buff_size);
    }

    DEBUG_SERIAL_PRINTF("Clients online: %d\n", ws.count());

  } else if (type == WS_EVT_DISCONNECT) {
    DEBUG_SERIAL_PRINTF("Clients online: %d\n", ws.count());
  } else if (type == WS_EVT_ERROR) {
    DEBUG_SERIAL_PRINTLN("Websocket error");
  } else if (type == WS_EVT_PONG) {

  } else if (WS_EVT_DATA) {
    DEBUG_SERIAL_PRINTF("Websocket data received: %u\n", client_id);
    receive_msg_and_perform_action(data, len);
  } else {
    DEBUG_SERIAL_PRINTLN("Websocket event not handled");
  }
}
