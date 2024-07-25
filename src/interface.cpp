#include "pump_control.h"
#include "str_num_msg_transcode.h"
#include "type_id.h"

void receive_control_data(uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }

  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
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
            "Received Control Data - Mode: %d\tRunning: %d\tResting: %d\n",
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
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  Strnum msg = Strnum_init_zero;
  if (deserialize_strnum(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", msg.str);
    DEBUG_SERIAL_PRINTF("Received number: %f\n", msg.num);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_strnum(msg);
  }
}

void receive_str(uint8_t *data, size_t len) {
  Str msg = Str_init_zero;
  if (deserialize_str(msg, data, len)) {
    DEBUG_SERIAL_PRINTF("Received string: %s\n", msg.value);
    DEBUG_SERIAL_PRINTF("Received key: %u\n", msg.key);

    free_str(msg);
  }
}

// void receive_num(uint8_t *data, size_t len)
void receive_single_config(uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }

  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    Num msg = Num_init_zero;

    if (!deserialize_num(msg, data, len)) {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize number message");
      xSemaphoreGive(controlDataMutex);
      return;
    }

    if (msg.key == 0) {
      DEBUG_SERIAL_PRINTLN("Received message with key 0, ignoring");
      xSemaphoreGive(controlDataMutex);
      return;
    }

    pump_ControlData &control_data = get_current_control_data();
    bool dataChanged = false;

    switch (static_cast<ConfigKey>(msg.key)) {
    case CONFIG_MODE:
      if (control_data.mode != static_cast<pump_MachineMode>(msg.value)) {
        control_data.mode = static_cast<pump_MachineMode>(msg.value);
        dataChanged = true;
      }
      break;
    case CONFIG_RUNNING_TIME:
      if (control_data.time_range.running != static_cast<uint32_t>(msg.value)) {
        control_data.time_range.running = static_cast<uint32_t>(msg.value);
        dataChanged = true;
      }
      break;
    case CONFIG_RESTING_TIME:
      if (control_data.time_range.resting != static_cast<uint32_t>(msg.value)) {
        control_data.time_range.resting = static_cast<uint32_t>(msg.value);
        dataChanged = true;
      }
      break;
    default:
      DEBUG_SERIAL_PRINTF("Received unknown key: %d\n", msg.key);
      xSemaphoreGive(controlDataMutex);
      return;
    }

    if (dataChanged) {
      if (ws.count() > 0) {
        ws.binaryAll(data, len);
      }

      if (msg.key == 2 || msg.key == 3) {
        xSemaphoreGive(controlDataMutex);
        store_time_range();
        return;
      }
    }

    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in receive_single_config()");
  }
}

void recieve_pump_time_range(uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received: NULL pointer or zero length");
    return;
  }
  if (xSemaphoreTake(controlDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
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
  if (data == NULL || len == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid data received");
    return;
  }
  if (data[0] < sizeof(receive_ptr) / sizeof(receive_ptr[0])) {
    receive_ptr[data[0]](data, len);
  } else {
    DEBUG_SERIAL_PRINTF("Invalid action index: %u\n", data[0]);
  }
}
