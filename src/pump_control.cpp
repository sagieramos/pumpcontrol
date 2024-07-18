#include "pump_control.h"
#include "msg_type_identifier.h"
#include "network.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t runMachineTask = NULL;
bool signalState = false;

void set_default_control_data(pump_ControlData *control_data) {
  control_data->mode = pump_MachineMode_AUTO;
  control_data->has_time_range = true;
  control_data->is_running = false;
  // control_data->time_range.running = TIME_ON;
  // control_data->time_range.resting = TIME_OFF;
}

void set_default_pump_time_range(pump_TimeRange *time_range) {
  time_range->running = TIME_ON;
  time_range->resting = TIME_OFF;
}

pump_ControlData &get_current_control_data() {
  static pump_ControlData control_data;
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
  pump_TimeRange time_range;
  pump_ControlData &control_data = get_current_control_data();

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

void controlPumpState() {
  pump_ControlData &ctrl = get_current_control_data();
  switch (ctrl.mode) {
  case pump_MachineMode::pump_MachineMode_AUTO: {
    if (signalState) {
      static unsigned long lastChangeTime = 0;
      unsigned long currentTime = getCurrentTimeMs();
      unsigned long signalTime = currentTime - lastChangeTime;
      if (signalTime >= ctrl.time_range.resting) {
        lastChangeTime = currentTime;
        // Turn Pump OFF after running
        if (switch_pump(false)) {
          ctrl.is_running = false;
          DEBUG_SERIAL_PRINTF("Pump ran in: %lums\n", signalTime);
        }
      } else if (signalTime >= ctrl.time_range.running) {
        // Keep Pump ON after running
        if (switch_pump(true)) {
          ctrl.is_running = true;
          DEBUG_SERIAL_PRINTF("Pump ran in: %lums\n", signalTime);
        }
      }
    } else {
      switch_pump(false);
    }
  } break;
  case pump_MachineMode::pump_MachineMode_POWER_ON:
    switch_pump(true);
    ctrl.is_running = true;
    break;
  case pump_MachineMode::pump_MachineMode_POWER_OFF:
    switch_pump(false);
    ctrl.is_running = false;
    break;
  default:
    DEBUG_SERIAL_PRINTLN("Invalid mode");
    break;
  }
}

void runMachine(void *parameter) {
  (void)parameter;
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  controlPumpState();

  for (;;) {
    int signal = 0;

    for (int i = 0; i < 5; i++) {
      signal += digitalRead(FLOAT_SIGNAL_PIN) == HIGH ? 1 : 0;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    signalState = signal >= 4;

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
  bool status = serialize_control_data(buffer, &buffer_size, &control_data,
                                       TYPE_IDENTIFIER_PUMP_CONTROL_DATA);
#ifndef PRODUCTION
  if (status) {
    for (size_t i = 0; i < encoded_size; i++) {
      DEBUG_SERIAL_PRINTF("%02X ", buffer[i]);
    }

    // Deserialize the data
    pump_ControlData control_data_deserialized;
    if (deserialize_control_data(buffer, encoded_size,
                                 &control_data_deserialized)) {
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

void setup_pump_controller() {
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  pump_ControlData &control_data = get_current_control_data();
  init_EEPROM_pump_controller();

  xTaskCreate(runMachine, "Pump Controller", 10000, NULL, 1, &runMachineTask);
}

/* int main() {
    uint8_t buffer[128];
    size_t encoded_size;

    pump_ControlData control_data;
    set_default_control_data(&control_data);

    if (serialize_control_data(buffer, sizeof(buffer), &encoded_size,
&control_data)) { printf("Serialized ControlData, size: %zu bytes\n",
encoded_size);

        pump_ControlData decoded_control_data = pump_ControlData_init_zero;
        if (deserialize_control_data(buffer, encoded_size,
&decoded_control_data)) { printf("Deserialized ControlData successfully\n");
            printf("Mode: %d\n", decoded_control_data.mode);
            if (decoded_control_data.has_time_range) {
                printf("Running Time: %u\n",
decoded_control_data.time_range.running); printf("Resting Time: %u\n",
decoded_control_data.time_range.resting);
            }
        } else {
            printf("Failed to deserialize ControlData\n");
        }
    } else {
        printf("Failed to serialize ControlData\n");
    }

    return 0;
}
 */