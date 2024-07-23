#include "pump_control.h"
#include "str_num_msg_transcode.h"
#include "type_id.h"

void receive_control_data(uint8_t *data, size_t len) {
  pump_ControlData control_data = get_current_control_data();

  if (deserialize_control_data(data, len, &control_data)) {
    if (ws.count() > 0) {
      ws.binaryAll(data, len);
    }
    store_pump_time_range();
  }
}

void void_action(uint8_t *data, size_t len) {
  (void)data;
  (void)len;
}

void receive_strnum(uint8_t *data, size_t len) {
  Strnum msg;
  if (deserialize_strnum(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", msg.str);
    DEBUG_SERIAL_PRINTF("Received number: %f\n", msg.num);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_strnum(msg);
  }
}

void receive_str(uint8_t *data, size_t len) {
  Str msg;
  if (deserialize_str(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", msg.value);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_str(msg);
  }
}

// void receive_num(uint8_t *data, size_t len)
void receive_single_config(uint8_t *data, size_t len) {
  Num msg;
  if (!deserialize_num(msg, data, len)) {
    return;
  }

  if (msg.key == 0) {
    return;
  }

  pump_ControlData control_data = get_current_control_data();

  switch (msg.key) {
  case 1:
    control_data.mode = static_cast<pump_MachineMode>(msg.value);
    break;
  case 2:
    control_data.time_range.running = static_cast<uint32_t>(msg.value);
    break;
  case 3:
    control_data.time_range.resting = static_cast<uint32_t>(msg.value);
    break;
  default:
    return;
  }

  if (ws.count() > 0) {
    ws.binaryAll(data, len);
  }

  if (msg.key == 2 || msg.key == 3) {
    store_pump_time_range();
  }
}

void recieve_pump_time_range(uint8_t *data, size_t len) {
  pump_TimeRange time_range;
  if (deserialize_time_range(data, len, time_range)) {
    pump_ControlData control_data = get_current_control_data();
    control_data.time_range = time_range;
    store_pump_time_range();

    if (ws.count() > 0) {
      ws.binaryAll(data, len);
    }
  }
}

MsgHandler receive_ptr[] = {
    void_action,            // Placeholder for index 0
    receive_single_config,  // Handle single configuration updates
    receive_str,            // Handle string messages
    receive_strnum,         // Handle string and number messages
    receive_control_data,   // Handle control data
    recieve_pump_time_range // Handle pump time range
};

void receive_msg_and_perform_action(uint8_t *data, size_t len) {
  if (data[0] < sizeof(receive_ptr) / sizeof(receive_ptr[0])) {
    receive_ptr[data[0]](data, len);
  } else {
    DEBUG_SERIAL_PRINTF("Invalid action index: %u\n", data[0]);
  }
}
