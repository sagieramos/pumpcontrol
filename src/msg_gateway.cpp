#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

TaskHandle_t msgGatewayTaskHandle = NULL;

static QueueHandle_t msgQueue = NULL;

static void send_num_message(const MessageData &msg_data, uint8_t type_id) {
  uint8_t buffer[Num_size];
  size_t buffer_size = Num_size;

  if (serialize_num(*msg_data.data.num, buffer, &buffer_size, type_id)) {
    LOG_F("Sent Num message. key: %d, value: %f | Type ID: %d | Buffer size: "
          "%d\n",
          msg_data.data.num->key, msg_data.data.num->value, type_id,
          buffer_size);

    send_binary_data(msg_data.client, buffer, buffer_size);
  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

static void send_control_data(const MessageData &msg_data, uint8_t type_id) {
  uint8_t buffer[pump_ControlData_size];
  size_t buffer_size = pump_ControlData_size;

  if (serialize_control_data(*msg_data.data.control_data, buffer, &buffer_size,
                             type_id)) {
    LOG_F("Sent ControlData message. | Type ID: %d | Buffer size: %d\n",
          type_id, buffer_size);
    send_binary_data(msg_data.client, buffer, buffer_size);
  } else {
    LOG_F("Failed to serialize ControlData message\n");
  }
}

static void send_msg1_data(const MessageData &msg_data, uint8_t type_id,
                           AsyncWebSocketClient *client = nullptr) {
  uint8_t buffer[Msg1_size];
  size_t buffer_size = Msg1_size;

  if (serialize_msg1(*msg_data.data.msg1, buffer, &buffer_size, type_id)) {
    LOG_F("Sent Msg1 data message. | Type ID: %d | Buffer size: %d\n", type_id,
          buffer_size);
    // Print the value of the message
    LOG_F("Msg1 data: f0: %f | f1: %f | f2: %f | f3: %f | f4: %f\n",
          msg_data.data.msg1->f0, msg_data.data.msg1->f1,
          msg_data.data.msg1->f2, msg_data.data.msg1->f3,
          msg_data.data.msg1->f4);
    send_binary_data(msg_data.client, buffer, buffer_size);
  } else {
    LOG_F("Failed to serialize Msg1 data message\n");
  }
}

bool enqueueMessage(const MessageData &msg_data) {
  if (xQueueSend(msgQueue, &msg_data, 0) != pdPASS) {
    // Queue is full, remove the oldest item (FIFO)
    LOG_LN("Queue full, removing oldest message to make space for new data");
    MessageData old_msg_data;
    if (xQueueReceive(msgQueue, &old_msg_data, 0) == pdPASS) {
      // Now space is available, try to enqueue the new message again
      if (xQueueSend(msgQueue, &msg_data, 0) != pdPASS) {
        LOG_LN(
            "Failed to enqueue the new message after removing the oldest one");
        return false;
      }
    } else {
      LOG_LN("Failed to dequeue the oldest message");
      return false;
    }
  }

  LOG_LN("Message enqueue successfully");
  return true;
}

void msgGatewayTask(void *pvParameters) {
  msgQueue = xQueueCreate(10, sizeof(MessageData));
  if (msgQueue == NULL) {
    LOG_LN("Failed to create message queue");
    vTaskDelete(NULL);
  }

  MessageData msg_data;

  for (;;) {
    if (xQueueReceive(msgQueue, &msg_data, portMAX_DELAY)) {
      switch (msg_data.data_type) {
      case MessageCommand::NUM:
        send_num_message(msg_data, msg_data.type_id);
        break;
      case MessageCommand::CONTROL_DATA:
        send_control_data(msg_data, msg_data.type_id);
        break;
      case MessageCommand::MSG1:
        send_msg1_data(msg_data, msg_data.type_id);
        break;
      default:
        LOG_LN("Unknown message command");
        break;
      }

      msg_data.client = nullptr;
    }
  }
}