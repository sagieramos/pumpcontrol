#include "pump_control.h"
#include "network.h"
#include "sensors.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t runMachineTask = NULL;
TaskHandle_t checkSignalTask = NULL;

bool pumpState = false;
unsigned long lastChangeTimeMs = 0;
float readingVolt = 0.0f;
bool powerOn = false;
bool automate_mode_signal = false;
bool flagP = false;

pump_ControlData current_pump_data = pump_ControlData_init_default;

TimerHandle_t delayTimer = NULL;

// Store time range in EEPROM
void store_time_range(bool check_changed) {
  EEPROM.begin(EEPROM_SIZE_CTL);

  bool time_range_changed = true; // Assume change by default

  if (check_changed) {
    pump_TimeRange stored_time_range;
    EEPROM.get(MAGIC_NUMBER_SIZE, stored_time_range);

    time_range_changed =
        (stored_time_range.running != current_pump_data.time_range.running) ||
        (stored_time_range.resting != current_pump_data.time_range.resting);
  }

  if (time_range_changed) {
    DEBUG_SERIAL_PRINTF("Storing Time Range - Running: %lu "
                        "milliseconds\tResting: %lu milliseconds\n",
                        current_pump_data.time_range.running,
                        current_pump_data.time_range.resting);

    EEPROM.put(MAGIC_NUMBER_SIZE, current_pump_data.time_range);
    EEPROM.commit();
    DEBUG_SERIAL_PRINTLN("Time Range updated in EEPROM");
  } else {
    DEBUG_SERIAL_PRINTLN("Time Range is unchanged, not updating EEPROM");
  }

  EEPROM.end();
}

// Validate if the time range is within acceptable limits (in milliseconds)
bool is_valid_time_range(const pump_TimeRange &time_range) {
  const unsigned long min_time_ms = 300000; // 5 minutes in milliseconds
  const unsigned long max_time_running_ms = 7200000; // 2 hours in milliseconds
  const unsigned long max_time_resting_ms =
      86400000; // 24 hours in milliseconds

  return (time_range.running >= min_time_ms &&
          time_range.running <= max_time_running_ms &&
          time_range.resting >= min_time_ms &&
          time_range.resting <= max_time_resting_ms);
}

// Initialize EEPROM with pump controller settings
void init_EEPROM_pump_controller() {
  EEPROM.begin(EEPROM_SIZE_CTL);

  uint8_t readMagicNumber;
  EEPROM.get(0, readMagicNumber);

  pump_TimeRange pump_time_range = pump_TimeRange_init_default;

  if (readMagicNumber == MAGIC_NUMBER) {
    EEPROM.get(MAGIC_NUMBER_SIZE, pump_time_range);
    DEBUG_SERIAL_PRINTF("Magic number found: %d\n", readMagicNumber);
    DEBUG_SERIAL_PRINTF("Read Time Range - Running: %lu milliseconds\tResting: "
                        "%lu milliseconds\n",
                        pump_time_range.running, pump_time_range.resting);

    // Validate and set time range
    if (is_valid_time_range(pump_time_range)) {
      current_pump_data.time_range = pump_time_range;
    } else {
      DEBUG_SERIAL_PRINTLN("Invalid time range, using default values");
      current_pump_data.time_range = pump_TimeRange_init_default;
    }
  } else {
    DEBUG_SERIAL_PRINTF("Magic number not found: %d\n", readMagicNumber);

    // Store default values and magic number
    EEPROM.put(0, MAGIC_NUMBER);
    EEPROM.put(MAGIC_NUMBER_SIZE, pump_time_range);
    EEPROM.commit();
  }

  EEPROM.end();
}

void updatePumpState(unsigned long signalTimeMs, unsigned long currentTimeMs) {
  readingVolt = readVoltage();
  if (readingVolt < min_voltage) {
    DEBUG_SERIAL_PRINTF("Supply voltage(%fV) is below required voltage(%fV)\n",
                        readingVolt, min_voltage);
    powerOn = false;
    flagP = true;
    return;
  }

  if (flagP) {
    DEBUG_SERIAL_PRINTF("Manually power ON");
    powerOn = true;
    flagP = false;
    if (!current_pump_data.is_running) {
      lastChangeTimeMs = currentTimeMs;
    }
  } else if (current_pump_data.is_running) {
    // Check if running time has elapsed
    DEBUG_SERIAL_PRINTF("Running Time remaining...: %lu milliseconds\n",
                        current_pump_data.time_range.running - signalTimeMs);
    if (signalTimeMs > current_pump_data.time_range.running) {
      powerOn = false;
      lastChangeTimeMs = currentTimeMs;
    }
  } else {
    // Check if resting time has elapsed
    DEBUG_SERIAL_PRINTF("Resting Time remaining...: %lu milliseconds\n",
                        current_pump_data.time_range.resting - signalTimeMs);
    if (signalTimeMs > current_pump_data.time_range.resting) {
      powerOn = true;
      lastChangeTimeMs = currentTimeMs;
    }
  }
}

bool anotherFlag = false;

// Control pump state based on mode and signal
void controlPumpState() {
  unsigned long currentTimeMs = getCurrentTimeMs();

  // Handle POWER_OFF mode
  if (current_pump_data.mode == pump_MachineMode_POWER_OFF) {
    powerOn = false;
    flagP = true;
    anotherFlag = true;
    switch_pump(powerOn);
    return;
  }

  // Compute time since the last state change
  unsigned long signalTimeMs = currentTimeMs - lastChangeTimeMs;

  // Handle AUTO and POWER_ON modes
  if (current_pump_data.mode == pump_MachineMode_AUTO) {
    if (automate_mode_signal) {
      updatePumpState(signalTimeMs, currentTimeMs);

    } else if (anotherFlag) {
      update_and_send_power_status(
          static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
      anotherFlag = false;
    } else {
      powerOn = false;
      flagP = true;
    }
  } else if (current_pump_data.mode == pump_MachineMode_POWER_ON) {
    updatePumpState(signalTimeMs, currentTimeMs);
    anotherFlag = true;
  } else {
    DEBUG_SERIAL_PRINTLN("Invalid mode");
  }

  switch_pump(powerOn);
}

// Task to run the machine
void runMachine(void *parameter) {
  (void)parameter;
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  init_EEPROM_pump_controller();

  for (;;) {
    controlPumpState();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task that checks signal state
void checkSignal(void *parameter) {
  (void)parameter;
  for (;;) {
    int signal = 0;
    for (int i = 0; i < 5; i++) {
      signal += digitalRead(FLOAT_SIGNAL_PIN) == HIGH ? 1 : 0;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    DEBUG_SERIAL_PRINTF("Signal: %d\n", signal);
    automate_mode_signal = signal >= 3;
    DEBUG_SERIAL_PRINTF("Current Mode: %d\n", current_pump_data.mode);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
