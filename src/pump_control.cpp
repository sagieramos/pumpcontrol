#include "pump_control.h"
#include "network.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t runMachineTask = NULL;

// Get the current control data
pump_ControlData &get_current_control_data() {
  static pump_ControlData control_data = pump_ControlData_init_default;
  return control_data;
}

// Store time range in EEPROM
void store_time_range() {
  pump_ControlData &control_data = get_current_control_data();
  DEBUG_SERIAL_PRINTF("Storing Time Range - Running: %d\tResting: %d\n",
                      control_data.time_range.running,
                      control_data.time_range.resting);
  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.put(MAGIC_NUMBER_SIZE, control_data.time_range);
  EEPROM.commit();
  EEPROM.end();
}

// Initialize EEPROM with pump controller settings
void init_EEPROM_pump_controller() {
  pump_ControlData &control_data = get_current_control_data();

  EEPROM.begin(EEPROM_SIZE_CTL);
  uint8_t readMagicNumber;
  EEPROM.get(0, readMagicNumber);

  if (readMagicNumber == MAGIC_NUMBER) {
    EEPROM.get(MAGIC_NUMBER_SIZE, control_data.time_range);
    DEBUG_SERIAL_PRINTF("Magic number found: %d\n", readMagicNumber);
    DEBUG_SERIAL_PRINTF("Read Time Range - Running: %d\tResting: %d\n",
                        control_data.time_range.running,
                        control_data.time_range.resting);
  } else {
    DEBUG_SERIAL_PRINTF("Magic number not found: %d\n", readMagicNumber);

    EEPROM.put(0, MAGIC_NUMBER);
    EEPROM.put(MAGIC_NUMBER_SIZE, control_data.time_range);
    EEPROM.commit();
  }

  EEPROM.end();
}

// Switch the pump state (ON/OFF)
bool switch_pump(bool state) {
  static bool pumpState = false;

  if (state != pumpState) {
    pumpState = state;
    digitalWrite(PUMP_RELAY_PIN, state ? HIGH : LOW);
    DEBUG_SERIAL_PRINTLN(state ? "Pump is ON" : "Pump is OFF");
    return true;
  }

  return false;
}

// Control pump state based on mode and signal
void controlPumpState(bool trigger_auto_pump) {
  pump_ControlData &ctrl = get_current_control_data();
  bool pumpStateChanged = false;
  static unsigned long lastChangeTime = 0;

  switch (ctrl.mode) {
  case pump_MachineMode_AUTO: {
    if (trigger_auto_pump) {
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
    break;
  }

  case pump_MachineMode_POWER_ON:
    if (switch_pump(true)) { // Turn Pump ON
      ctrl.is_running = true;
      pumpStateChanged = true;
    }
    break;

  case pump_MachineMode_POWER_OFF:
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
    msg.key = 4;
    msg.value = static_cast<float>(ctrl.is_running);

    uint8_t buffer[128];
    size_t buffer_size = sizeof(buffer);
    serialize_num(msg, buffer, &buffer_size, SINGLE_CONFIG_TYPE_ID,
                  send_binary_data);

    // Send the data (use a separate function for sending if necessary)
    if (ws.count() > 0) {
      ws.binaryAll(buffer, buffer_size);
    }
  }

#ifndef PRODUCTION
  if (pumpStateChanged) {
    DEBUG_SERIAL_PRINTF("Pump state changed in: %lums\n",
                        getCurrentTimeMs() - lastChangeTime);
    DEBUG_SERIAL_PRINTF("Pump is %s\n", ctrl.is_running ? "ON" : "OFF");
  }
#endif
}

// Task to run the machine
void runMachine(void *parameter) {
  (void)parameter;
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

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

// Send control data to the specified client
void send_control_data(const size_t client_id) {
  if (client_id == 0)
    return;

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

    // Deserialize the data for verification
    pump_ControlData control_data_deserialized;
    if (deserialize_control_data(control_data_deserialized, buffer,
                                 encoded_size)) {
      DEBUG_SERIAL_PRINTLN("Deserialized data:");
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

  if (status) {
    if (client_id == 0) {
      ws.binaryAll(buffer, encoded_size);
    } else {
      ws.binary(client_id, buffer, encoded_size);
    }
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize control data");
  }
}
