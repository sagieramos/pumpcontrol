#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "str_num_msg_transcode.h"
#include "type_id.h"

#define SEMAPHORE_TIMEOUT_MS 1000

void send_num_message(Num value, uint8_t type_id) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(value, buffer, &buffer_size, type_id, send_binary_data)) {
    DEBUG_SERIAL_PRINTF("Sent Num message. key: %d, value: %f | Type ID: %d | "
                        "Buffer size: %d\n",
                        value.key, value.value, type_id, buffer_size);
  } else {
    DEBUG_SERIAL_PRINTF("Failed to serialize power message\n");
  }
}

void send_num_message_to_a_client(Num value, uint8_t type_id,
                                  AsyncWebSocketClient *client) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(value, buffer, &buffer_size, type_id)) {
    DEBUG_SERIAL_PRINTF("Sent Num message. key: %d, value: %f | Type ID: %d | "
                        "Buffer size: %d\n",
                        value.key, value.value, type_id, buffer_size);
    client->binary(buffer, buffer_size);
  } else {
    DEBUG_SERIAL_PRINTF("Failed to serialize power message\n");
  }
}

void receive_control_data(uint8_t *data, size_t len) {
  DEBUG_SERIAL_PRINTF("Received `control_data` message of length %u\n", len);
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }

  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(SEMAPHORE_TIMEOUT_MS)) ==
      pdTRUE) {
    pump_ControlData &control_data = get_current_control_data();
    pump_ControlData new_control_data = control_data;

    if (deserialize_control_data(new_control_data, data, len)) {
      bool data_changed = (control_data.mode != new_control_data.mode) ||
                          (control_data.time_range.running !=
                           new_control_data.time_range.running) ||
                          (control_data.time_range.resting !=
                           new_control_data.time_range.resting);

      if (data_changed) {
        control_data = new_control_data;

        if (ws.count() > 0) {
          ws.binaryAll(data, len);
        }

        DEBUG_SERIAL_PRINTF(
            "Received Control Data - Mode: %d\tRunning: %lu\tResting: %lu\n",
            control_data.mode, control_data.time_range.running,
            control_data.time_range.resting);

        xSemaphoreGive(controlDataMutex);

        store_time_range();

        return;
      } else {
        DEBUG_SERIAL_PRINTLN(
            "Received data is the same as the current data; no update needed.");
      }

      xSemaphoreGive(controlDataMutex);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize control data");
      xSemaphoreGive(controlDataMutex);
    }
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in receive_control_data()");
  }
}

void void_action(uint8_t *data, size_t len) {
  (void)data;
  (void)len;
}

void receive_strnum(uint8_t *data, size_t len) {
  DEBUG_SERIAL_PRINTF("Received `strnum` message of length %u\n", len);
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  Strnum msg = Strnum_init_zero;
  if (deserialize_strnum(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", (const char *)msg.str.arg);
    DEBUG_SERIAL_PRINTF("Received number: %f\n", msg.num);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_strnum(msg);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize string and number message");
  }
}

void receive_str(uint8_t *data, size_t len) {
  DEBUG_SERIAL_PRINTF("Received `str` message of length %u\n", len);
  Str msg = Str_init_zero;
  if (deserialize_str(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", (const char *)msg.value.arg);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_str(msg);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize string message");
  }
}

void receive_min_voltage(uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  Num msg = Num_init_zero;
  if (deserialize_num(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received min voltage: %f\n", msg.value);
    if (store_min_voltage(msg.value)) {
      if (ws.count() > 0) {
        ws.binaryAll(data, len);
      }
    }
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize min voltage message");
  }
}
bool deserialize_and_validate(Num &msg, uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return false;
  }

  if (!deserialize_num(msg, data, len)) {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize number message");
    return false;
  }

  if (msg.key == 0 || msg.key >= ConfigKey::CONFIG_KEY_MAX) {
    DEBUG_SERIAL_PRINTF("Invalid key: %d\n", msg.key);
    return false;
  }

  return true;
}

void handle_min_voltage(Num &msg, uint8_t *data, size_t len,
                        bool &dataChanged) {
  DEBUG_SERIAL_PRINTF("Current min voltage: %f\n", min_voltage);
  if (min_voltage != msg.value) {
    store_min_voltage(msg.value);
    dataChanged = true;
    DEBUG_SERIAL_PRINTF("Received min voltage: %f\n", msg.value);

    if (ws.count() > 0) {
      ws.binaryAll(data, len);
    }
  }
}

void handle_control_data_update(Num &msg, bool &dataChanged) {
  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(SEMAPHORE_TIMEOUT_MS)) !=
      pdTRUE) {
    DEBUG_SERIAL_PRINTF(
        "Failed to acquire controlDataMutex in receive_single_config()");
    return;
  }

  pump_ControlData &control_data = get_current_control_data();

  switch (static_cast<ConfigKey>(msg.key)) {
  case CONFIG_MODE:
    DEBUG_SERIAL_PRINTF("Current mode: %d\n", control_data.mode);
    if (control_data.mode != static_cast<pump_MachineMode>(msg.value)) {
      control_data.mode = static_cast<pump_MachineMode>(msg.value);
      dataChanged = true;
      DEBUG_SERIAL_PRINTF("Received mode: %d\n", msg.value);
    }
    break;

  case CONFIG_RUNNING_TIME:
    DEBUG_SERIAL_PRINTF("Current running time: %d\n",
                        control_data.time_range.running);
    if (control_data.time_range.running != static_cast<uint32_t>(msg.value)) {
      control_data.time_range.running = static_cast<uint32_t>(msg.value);
      dataChanged = true;
      store_time_range();
      DEBUG_SERIAL_PRINTF("Received running time: %d\n", msg.value);
    }
    break;

  case CONFIG_RESTING_TIME:
    DEBUG_SERIAL_PRINTF("Current resting time: %d\n",
                        control_data.time_range.resting);
    if (control_data.time_range.resting != static_cast<uint32_t>(msg.value)) {
      control_data.time_range.resting = static_cast<uint32_t>(msg.value);
      dataChanged = true;
      store_time_range();
      DEBUG_SERIAL_PRINTF("Received resting time: %d\n", msg.value);
    }
    break;

  default:
    DEBUG_SERIAL_PRINTF("Received unknown key: %d\n", msg.key);
    break;
  }

  xSemaphoreGive(controlDataMutex);
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
  DEBUG_SERIAL_PRINTF("Received config message of length %u\n", len);

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
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(SEMAPHORE_TIMEOUT_MS)) ==
      pdTRUE) {
    pump_TimeRange time_range = pump_TimeRange_init_zero;
    if (deserialize_time_range(time_range, data, len)) {
      pump_ControlData &control_data = get_current_control_data();
      control_data.time_range = time_range;

      DEBUG_SERIAL_PRINTF("Received Time Range - Running: %d\tResting: %d\n",
                          time_range.running, time_range.resting);
      DEBUG_SERIAL_PRINTF("Current Time Range - Running: %d\tResting: %d\n",
                          control_data.time_range.running,
                          control_data.time_range.resting);
      if (ws.count() > 0) {
        ws.binaryAll(data, len);
      }
      xSemaphoreGive(controlDataMutex);
      store_time_range();
      return;
    }

    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in receive_pump_time_range()");
  }
}

void receive_auth(uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  Auth msg = Auth_init_zero;
  if (deserialize_auth(msg, data, len, NULL)) {
    DEBUG_SERIAL_PRINTF("Received ID: %s\n", (const char *)msg.id.arg);
    DEBUG_SERIAL_PRINTF("Received Password: %s\n", (const char *)msg.pass.arg);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize Auth message");
  }
}

void receive_msg_and_perform_action(uint8_t *data, size_t len,
                                    uint8_t msg_type) {
  DEBUG_SERIAL_PRINTF("Received `msg` of length %u and type: %d\n", len,
                      msg_type);

  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }

  void (*receive_ptr[])(uint8_t *, size_t) = {
      void_action,             // 0
      receive_single_config,   // 1
      receive_str,             // 2
      receive_strnum,          // 3
      receive_control_data,    // 4
      receive_pump_time_range, // 5
      receive_auth,            // 6
      receive_min_voltage,     // 7
  };

  const uint8_t len_msg_types = sizeof(receive_ptr) / sizeof(receive_ptr[0]);

  if (msg_type >= len_msg_types) {
    DEBUG_SERIAL_PRINTF("Unhandled message type: %d\n", msg_type);
    return;
  }

  receive_ptr[msg_type](data, len);
}
