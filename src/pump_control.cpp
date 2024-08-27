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
    EEPROM.get(PUMP_TIME_RANGE_ADDRESS, stored_time_range);

    time_range_changed =
        (stored_time_range.running != current_pump_data.time_range.running) ||
        (stored_time_range.resting != current_pump_data.time_range.resting);
  }

  if (time_range_changed) {
    LOG_F("Storing Time Range - Running: %lu "
          "milliseconds\tResting: %lu milliseconds\n",
          current_pump_data.time_range.running,
          current_pump_data.time_range.resting);

    EEPROM.put(PUMP_TIME_RANGE_ADDRESS, current_pump_data.time_range);
    EEPROM.commit();
    LOG_LN("Time Range updated in EEPROM");
  } else {
    LOG_LN("Time Range is unchanged, not updating EEPROM");
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
    EEPROM.get(PUMP_TIME_RANGE_ADDRESS, pump_time_range);
    LOG_F("Magic number found: %d\n", readMagicNumber);
    LOG_F("Read Time Range - Running: %lu milliseconds\tResting: "
          "%lu milliseconds\n",
          pump_time_range.running, pump_time_range.resting);

    // Validate and set time range
    if (is_valid_time_range(pump_time_range)) {
      current_pump_data.time_range = pump_time_range;
    } else {
      LOG_LN("Invalid time range, using default values");
      current_pump_data.time_range = pump_TimeRange_init_default;
    }

    // Read pump mode from eeprom
    uint8_t pump_mode = 0;
    EEPROM.get(PUMP_MODE_ADDRESS, pump_mode);
    // pump mode can only be AUTO or POWER_OFF
    if (pump_mode == pump_MachineMode_AUTO ||
        pump_mode == pump_MachineMode_POWER_OFF) {
      current_pump_data.mode = static_cast<pump_MachineMode>(pump_mode);
    } else {
      LOG_LN("Invalid pump mode, using default value");
      current_pump_data.mode = pump_MachineMode_AUTO;
    }

  } else {
    LOG_F("Magic number not found: %d\n", readMagicNumber);

    // Store default values and magic number
    EEPROM.put(0, MAGIC_NUMBER);
    EEPROM.put(PUMP_TIME_RANGE_ADDRESS, pump_time_range);
    EEPROM.commit();
  }

  EEPROM.end();
}

// check and store pump mode in EEPROM
void store_pump_mode(bool check_changed) {
  EEPROM.begin(EEPROM_SIZE_CTL);

  if (check_changed) {
    uint8_t stored_pump_mode;
    EEPROM.get(PUMP_MODE_ADDRESS, stored_pump_mode);
    if (stored_pump_mode != current_pump_data.mode) {
      EEPROM.put(PUMP_MODE_ADDRESS, current_pump_data.mode);
      EEPROM.commit();
      LOG_F("Stored pump mode: %d\n", current_pump_data.mode);
    }
  } else {
    EEPROM.put(PUMP_MODE_ADDRESS, current_pump_data.mode);
    EEPROM.commit();
    LOG_F("Stored pump mode: %d\n", current_pump_data.mode);
  }

  EEPROM.end();
}

void updatePumpState(unsigned long signalTimeMs, unsigned long currentTimeMs) {
  readingVolt = readVoltage();
  if (readingVolt < min_voltage) {
    LOG_F("Supply voltage(%fV) is below required voltage(%fV)\n", readingVolt,
          min_voltage);
    powerOn = false;
    flagP = true;
    return;
  }

  if (flagP) {
    LOG_F("Manually power ON");
    powerOn = true;
    flagP = false;
    if (!current_pump_data.is_running) {
      lastChangeTimeMs = currentTimeMs;
    }
  } else if (current_pump_data.is_running) {
    // Check if running time has elapsed
    LOG_F("Running Time remaining...: %lu milliseconds\n",
          current_pump_data.time_range.running - signalTimeMs);
    if (signalTimeMs > current_pump_data.time_range.running) {
      powerOn = false;
      lastChangeTimeMs = currentTimeMs;
    }
  } else {
    // Check if resting time has elapsed
    LOG_F("Resting Time remaining...: %lu milliseconds\n",
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
    LOG_LN("Invalid mode");
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
    LOG_F("Signal: %d\n", signal);
    automate_mode_signal = signal >= 3;
    LOG_F("Current Mode: %d\n", current_pump_data.mode);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
