#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "str_num_msg_transcode.h"
#include "type_id.h"

#define SEMAPHORE_TIMEOUT_MS 1000

void send_num_message(Num &value, uint8_t type_id) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(value, buffer, &buffer_size, type_id, send_binary_data)) {
    LOG_F("Sent Num message. key: %d, value: %f | Type ID: %d | "
          "Buffer size: %d\n",
          value.key, value.value, type_id, buffer_size);
  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

void send_num_message_to_a_client(Num value, uint8_t type_id,
                                  AsyncWebSocketClient *client) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(value, buffer, &buffer_size, type_id)) {
    LOG_F("Sent Num message. key: %d, value: %f | Type ID: %d | "
          "Buffer size: %d\n",
          value.key, value.value, type_id, buffer_size);
    client->binary(buffer, buffer_size);

  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

void receive_control_data(uint8_t *data, size_t len) {
  pump_ControlData new_control_data = pump_ControlData_init_zero;

  if (deserialize_control_data(new_control_data, data, len)) {
    bool data_changed = (current_pump_data.mode != new_control_data.mode) ||
                        (current_pump_data.time_range.running !=
                         new_control_data.time_range.running) ||
                        (current_pump_data.time_range.resting !=
                         new_control_data.time_range.resting);

    if (data_changed) {
      current_pump_data = new_control_data;

      if (ws.count() > 0) {
        ws.binaryAll(data, len);
      }

      LOG_F("Received Control Data - Mode: %d\tRunning: %lu\tResting: %lu\n",
            current_pump_data.mode, current_pump_data.time_range.running,
            current_pump_data.time_range.resting);

      store_time_range(false);

      return;
    } else {
      LOG_LN(
          "Received data is the same as the current data; no update needed.");
    }

  } else {
    LOG_LN("Failed to deserialize control data");
  }
}

void void_action(uint8_t *data, size_t len) {
  (void)data;
  (void)len;
}

void receive_strnum(uint8_t *data, size_t len) {
  Strnum msg = Strnum_init_zero;
  if (deserialize_strnum(msg, data, len)) {
    LOG_F("Received string: %s\n", (const char *)msg.str.arg);
    LOG_F("Received number: %f\n", msg.num);
    LOG_F("Received key: %u\n", msg.key);

    free_strnum(msg);
  } else {
    LOG_LN("Failed to deserialize string and number message");
  }
}

void receive_str(uint8_t *data, size_t len) {
  LOG_F("Received `str` message of length %u\n", len);
  Str msg = Str_init_zero;
  if (deserialize_str(msg, data, len)) {
    LOG_F("Received string: %s\n", (const char *)msg.value.arg);
    LOG_F("Received key: %u\n", msg.key);

    free_str(msg);
  } else {
    LOG_LN("Failed to deserialize string message");
  }
}

void receive_min_voltage(uint8_t *data, size_t len) {
  Num msg = Num_init_zero;

  if (!deserialize_num(msg, data, len)) {
    LOG_LN("Failed to deserialize min voltage message");
    return;
  }

  LOG_F("Received min voltage: %f\n", msg.value);

  if (!store_min_voltage(msg.value)) {
    LOG_LN("Voltage not store in Flash Memory");
    return;
  }

  if (ws.count() > 0) {
    ws.binaryAll(data, len);
  }
}

bool deserialize_and_validate(Num &msg, uint8_t *data, size_t len) {
  if (!deserialize_num(msg, data, len)) {
    LOG_LN("Failed to deserialize number message");
    return false;
  }

  if (msg.key == 0 || msg.key >= ConfigKey::CONFIG_KEY_MAX) {
    LOG_F("Invalid key: %d\n", msg.key);
    return false;
  }

  return true;
}

void handle_min_voltage(Num &msg, uint8_t *data, size_t len,
                        bool &dataChanged) {
  LOG_F("Current min voltage: %f\n", min_voltage);
  if (min_voltage != msg.value) {
    store_min_voltage(msg.value);
    dataChanged = true;
    LOG_F("Received min voltage: %f\n", msg.value);

    if (ws.count() > 0) {
      ws.binaryAll(data, len);
    }
  }
}

void handle_control_data_update(Num &msg, bool &dataChanged) {
  switch (static_cast<ConfigKey>(msg.key)) {
  case CONFIG_MODE: {
    pump_MachineMode mode = current_pump_data.mode;
    pump_MachineMode new_mode = static_cast<pump_MachineMode>(msg.value);

    LOG_F("Current mode: %d\n", mode);

    bool f = new_mode >= 0 && new_mode <= 2;
    if (mode != new_mode && f) {
      current_pump_data.mode = new_mode;
      dataChanged = true;
      store_pump_mode(false);
      LOG_F("Received mode: %d\n", new_mode);
    } else {
      LOG_F("Received mode already selected or not valid: %d\n", new_mode);
    }
    break;
  }
  case CONFIG_RUNNING_TIME: {
    LOG_F("Current running time: %d\n", current_pump_data.time_range.running);
    uint32_t new_running_time = static_cast<uint32_t>(msg.value);
    if (current_pump_data.time_range.running != new_running_time) {
      current_pump_data.time_range.running = new_running_time;
      dataChanged = true;
      store_time_range(false);
      LOG_F("Received running time: %d\n", msg.value);
    }
    break;
  }
  case CONFIG_RESTING_TIME: {
    LOG_F("Current resting time: %d\n", current_pump_data.time_range.resting);
    uint32_t new_resting_time = static_cast<uint32_t>(msg.value);
    if (current_pump_data.time_range.resting != new_resting_time) {
      current_pump_data.time_range.resting = new_resting_time;
      dataChanged = true;
      store_time_range(false);
      LOG_F("Received resting time: %d\n", msg.value);
    }
    break;
  }
  default: {
    LOG_F("Received unknown key: %d\n", msg.key);
    break;
  }
  }
}

void send_data_if_changed(bool dataChanged, Num &msg, uint8_t *data,
                          size_t len) {
  if (dataChanged) {
    if (ws.count() > 0) {
      ws.binaryAll(data, len);
    }
  }
}

void receive_single_config(uint8_t *data, size_t len) {
  LOG_F("Received config message of length %u\n", len);

  Num msg = Num_init_zero;
  if (!deserialize_and_validate(msg, data, len)) {
    return;
  }

  bool dataChanged = false;

  switch (static_cast<ConfigKey>(msg.key)) {
  case CONFIG_MIN_VOLTAGE:
    handle_min_voltage(msg, data, len, dataChanged);
    return;

  default:
    handle_control_data_update(msg, dataChanged);
    break;
  }

  send_data_if_changed(dataChanged, msg, data, len);
}

void receive_pump_time_range(uint8_t *data, size_t len) {
  pump_TimeRange time_range = pump_TimeRange_init_zero;
  if (!deserialize_time_range(time_range, data, len)) {
    LOG_LN("Failed to deserialize time range message");
    return;
  }

  if (!is_valid_time_range(time_range)) {
    LOG_LN("Invalid time range received");
    return;
  }

  // Check if the new time_range is the same as the current one
  if (current_pump_data.time_range.running == time_range.running &&
      current_pump_data.time_range.resting == time_range.resting) {
    LOG_LN("Received Time Range is the same as the current "
           "range. No update required.");
    return;
  }

  // Update control data with the new time range
  current_pump_data.time_range = time_range;

  LOG_F("Received Time Range - Running: %ld\tResting: %ld\n",
        time_range.running, time_range.resting);
  LOG_F("Current Time Range - Running: %ld\tResting: %ld\n",
        current_pump_data.time_range.running,
        current_pump_data.time_range.resting);

  if (ws.count() > 0) {
    ws.binaryAll(data, len);
  }

  send_all_power_status_and_type();

  store_time_range(false);
}

void receive_auth(uint8_t *data, size_t len) {
  Auth msg = Auth_init_zero;
  if (deserialize_auth(msg, data, len, NULL)) {
    LOG_F("Received ID: %s\n", (const char *)msg.id.arg);
    LOG_F("Received Password: %s\n", (const char *)msg.pass.arg);
  } else {
    LOG_LN("Failed to deserialize Auth message");
  }
}

void receive_msg_and_perform_action(uint8_t *data, size_t len,
                                    uint8_t msg_type) {
  if (data == NULL || len == 0) {
    LOG_LN("Invalid data received: NULL pointer or zero length");
    return;
  }

  LOG_F("Received `msg` of length %u and type: %d\n", len, msg_type);

  constexpr void (*receive_ptr[])(uint8_t *, size_t) = {
      void_action,             // 0: For No-operation actions
      receive_single_config,   // 1: For Receiving single configuration updates
      receive_str,             // 2: For Receiving string messages
      receive_strnum,          // 3: For Receiving string and number messages
      receive_control_data,    // 4: For Setting control data
      receive_pump_time_range, // 5: For Setting pump time range
      receive_auth,            // 6: For Authentication
      receive_min_voltage,     // 7: For Setting min voltage
  };

  constexpr uint8_t len_msg_types =
      sizeof(receive_ptr) / sizeof(receive_ptr[0]);

  if (msg_type >= len_msg_types) {
    LOG_F("Unhandled message type: %d\n", msg_type);
    return;
  }

  receive_ptr[msg_type](data, len);
}
