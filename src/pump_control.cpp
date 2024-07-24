#include "pump_control.h"
#include "network.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t runMachineTask = NULL;

void set_default_control_data(pump_ControlData *control_data) {
  control_data->mode = pump_MachineMode_AUTO;
  control_data->has_time_range = true;
  control_data->time_range.running = TIME_ON;
  control_data->time_range.resting = TIME_OFF;
}

void set_default_pump_time_range(pump_TimeRange *time_range) {
  time_range->running = TIME_ON;
  time_range->resting = TIME_OFF;
}

pump_ControlData &get_current_control_data() {
  static pump_ControlData control_data = pump_ControlData_init_zero;
  return control_data;
}

void store_time_range(pump_TimeRange *time_range) {
  DEBUG_SERIAL_PRINTF("Storing Time Range - Running: %d\tResting: %d\n",
                      time_range->running, time_range->resting);
  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.put(MAGIC_NUMBER_SIZE, *time_range);
  EEPROM.commit();
  EEPROM.end();
}

void init_EEPROM_pump_controller() {
  pump_TimeRange time_range = pump_TimeRange_init_zero;
  pump_ControlData &control_data = get_current_control_data();
  control_data.has_time_range = true;

  EEPROM.begin(EEPROM_SIZE_CTL);

  uint32_t readMagicNumber;
  EEPROM.get(0, readMagicNumber);
  if (readMagicNumber == MAGIC_NUMBER) {
    EEPROM.get(MAGIC_NUMBER_SIZE, time_range);
    control_data.time_range = time_range;
    DEBUG_SERIAL_PRINTF("Read Time Range - Running: %d\tResting: %d\n",
                        time_range.running, time_range.resting);
    EEPROM.end();
  } else {
    set_default_pump_time_range(&time_range);
    control_data.time_range = time_range;
    store_time_range(&time_range);
  }
}

void store_pump_time_range(const pump_TimeRange *time_range) {
  pump_ControlData &control_data = get_current_control_data();
  control_data.time_range = *time_range;
  store_time_range(&control_data.time_range);
}

void store_pump_time_range() {
  pump_ControlData &control_data = get_current_control_data();
  store_time_range(&control_data.time_range);
}

bool switch_pump(bool state) {
  static bool pumpState = false;

  if (state != pumpState) {
    pumpState = state;
    if (state) {
      digitalWrite(PUMP_RELAY_PIN, HIGH);
    } else {
      digitalWrite(PUMP_RELAY_PIN, LOW);
    }
    DEBUG_SERIAL_PRINTLN(state ? "Pump is ON" : "Pump is OFF");

    return true;
  }

  return false;
}

void controlPumpState(bool triger_auto_pump) {
  pump_ControlData &ctrl = get_current_control_data();

  bool pumpStateChanged = false;
  static unsigned long lastChangeTime = 0;

  switch (ctrl.mode) {
  case pump_MachineMode::pump_MachineMode_AUTO: {
    if (triger_auto_pump) {
      unsigned long currentTime = getCurrentTimeMs();
      unsigned long signalTime = currentTime - lastChangeTime;

      if (signalTime >= ctrl.time_range.resting) {
        lastChangeTime = currentTime;
        if (switch_pump(false)) { // Turn Pump OFF
          ctrl.is_running = false;
          pumpStateChanged = true;
        }
      } else if (signalTime >= ctrl.time_range.running) {
        if (switch_pump(true)) { // Keep Pump ON
          ctrl.is_running = true;
          pumpStateChanged = true;
        }
      }
    } else {
      if (switch_pump(false)) { // Turn Pump OFF
        ctrl.is_running = false;
        pumpStateChanged = true;
      }
    }
  } break;

  case pump_MachineMode::pump_MachineMode_POWER_ON:
    if (switch_pump(true)) { // Turn Pump ON
      ctrl.is_running = true;
      pumpStateChanged = true;
    }
    break;

  case pump_MachineMode::pump_MachineMode_POWER_OFF:
    if (switch_pump(false)) { // Turn Pump OFF
      ctrl.is_running = false;
      pumpStateChanged = true;
    }
    break;

  default:
    DEBUG_SERIAL_PRINTLN("Invalid mode");
    break;
  }

  if (pumpStateChanged) {
    Num msg;
    pump_ControlData &control_data = get_current_control_data();
    msg.key = 4;
    msg.value = static_cast<float>(control_data.is_running);
    uint8_t buffer[128];
    size_t buffer_size = sizeof(buffer);
    serialize_num(msg, buffer, &buffer_size, SINGLE_CONFIG_TYPE_ID,
                  send_binary_data);
    pumpStateChanged = false;
  }

#ifndef PRODUCTION
  if (pumpStateChanged) {
    DEBUG_SERIAL_PRINTF("Pump state changed in: %lums\n",
                        getCurrentTimeMs() - lastChangeTime);
    DEBUG_SERIAL_PRINTF("Pump is %s\n", ctrl.is_running ? "ON" : "OFF");
  }
#endif
}

void runMachine(void *parameter) {
  (void)parameter;
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  pump_ControlData &control_data = get_current_control_data();
  control_data.mode = pump_MachineMode_AUTO;
  init_EEPROM_pump_controller();

  for (;;) {
    int signal = 0;

    for (int i = 0; i < 5; i++) {
      signal += digitalRead(FLOAT_SIGNAL_PIN) == HIGH ? 1 : 0;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    controlPumpState(signal >= 4);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void send_control_data(const size_t client_id) {
  if (client_id == 0) {
    return;
  }
  uint8_t buffer[1024];
  size_t buffer_size = sizeof(buffer);

  pump_ControlData control_data = get_current_control_data();
  DEBUG_SERIAL_PRINTF("Control Data - Mode: %d\tRunning: %d\tTime Range - "
                      "Running: %d\tResting: %d\n",
                      control_data.mode, control_data.is_running,
                      control_data.time_range.running,
                      control_data.time_range.resting);
  size_t encoded_size;
  bool status = serialize_control_data(control_data, buffer, &buffer_size,
                                       CONTROL_DATA_TYPE_ID);
#ifndef PRODUCTION
  if (status) {
    for (size_t i = 0; i < encoded_size; i++) {
      DEBUG_SERIAL_PRINTF("%02X ", buffer[i]);
    }

    // Deserialize the data
    pump_ControlData control_data_deserialized;
    if (deserialize_control_data(control_data_deserialized, buffer,
                                 encoded_size)) {
      DEBUG_SERIAL_PRINTLN("Deserialized data..............................:");
      DEBUG_SERIAL_PRINTF("Mode: %d\n", control_data_deserialized.mode);
      DEBUG_SERIAL_PRINTF("Running: %d\n",
                          control_data_deserialized.is_running);
      DEBUG_SERIAL_PRINTF("Time Range - Running: %d\n",
                          control_data_deserialized.time_range.running);
      DEBUG_SERIAL_PRINTF("Time Range - Resting: %d\n",
                          control_data_deserialized.time_range.resting);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize control data");
    }
    DEBUG_SERIAL_PRINTLN();
  }
#endif

  if (status && client_id == 0) {
    ws.binaryAll(buffer, encoded_size);
  } else if (status) {
    ws.binary(client_id, buffer, encoded_size);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize control data");
  }
}