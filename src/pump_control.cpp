#include "pump_control.h"
#include "network.h"
#include "sensors.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

const int SIGNAL_READ_COUNT = 5;
const int SIGNAL_READ_DELAY_MS = 500;

TaskHandle_t runMachineTaskHandle = NULL;
TaskHandle_t checkSignalHandle = NULL;
TaskHandle_t readPzemTaskHandle = NULL;

unsigned long lastChangeTimeMs = 0;
bool powerOn = false;
bool automate_mode_signal = false;
uint32_t previous_power_status = 0;

pump_ControlData current_pump_data = pump_ControlData_init_default;

TimerHandle_t delayTimer = NULL;

void clearEEPROM() {
  EEPROM.begin(EEPROM_SIZE_CTL);
  for (int i = 0; i < EEPROM_SIZE_CTL; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
}

// Store time range in EEPROM
void store_time_range(bool check_changed) {
  EEPROM.begin(EEPROM_SIZE_CTL);

  bool time_range_changed = true;

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
  bool timeElapsed = false;
  if (current_pump_data.is_running) {
    timeElapsed = signalTimeMs > current_pump_data.time_range.running;
  } else {
    timeElapsed = signalTimeMs > current_pump_data.time_range.resting;
  }

  if (timeElapsed) {
    powerOn = !current_pump_data.is_running;
    lastChangeTimeMs = currentTimeMs;
  }
}

void running_or_resting_and_update(unsigned long signalTimeMs,
                                   unsigned long currentTimeMs) {
  switch (power.key) {
  case PowerStatus::POWER_VOLTAGE_LOW:
    power.key = previous_power_status;
    if (power.key == PowerStatus::POWER_RESTING)
      sendCurrentMachineState();
    updatePumpState(signalTimeMs, currentTimeMs);
    break;

  case PowerStatus::POWER_RESTING:
  case PowerStatus::POWER_RUNNING:
    updatePumpState(signalTimeMs, currentTimeMs);
    break;

  default:
    powerOn = true;
    lastChangeTimeMs = currentTimeMs;
    break;
  }
}

#ifndef PRODUCTION
void logPumpRunningState() {
  static bool previousRunningState = current_pump_data.is_running;

  if (previousRunningState != current_pump_data.is_running) {
    LOG_F("Power is__________________________________ %s\n",
          current_pump_data.is_running ? "ON" : "OFF");
    previousRunningState = current_pump_data.is_running;
  }
}

void logPumpMode() {
  static pump_MachineMode previousMode = current_pump_data.mode;

  if (previousMode != current_pump_data.mode) {
    LOG_F("Current Mode: %d\n", current_pump_data.mode);
    previousMode = current_pump_data.mode;
  }
}

void logAutoSignal() {
  static bool previous = automate_mode_signal;

  if (automate_mode_signal != previous) {
    LOG_F("Automate sensor is %s\n", automate_mode_signal ? "HIGH" : "LOW");
    previous = automate_mode_signal;
  }
}
#endif

void controlPumpState() {
  static bool log_voltage_low = true;

  if (pzemTwoData.f0 < min_voltage) {
    switch_pump(false);
    static pump_MachineMode previousMode = current_pump_data.mode;
    static bool previous_automate_auto_signal = automate_mode_signal;

    if (log_voltage_low || previousMode != current_pump_data.mode ||
        previous_automate_auto_signal != automate_mode_signal) {
      LOG_F("Supply voltage(%fV) is below required voltage(%fV)\n",
            pzemTwoData.f0, min_voltage);

      if (log_voltage_low) {
        previous_power_status = power.key;
      }

      if (current_pump_data.mode == pump_MachineMode_POWER_OFF ||
          (current_pump_data.mode == pump_MachineMode_AUTO &&
           !automate_mode_signal)) {
        update_and_send_power_status(
            static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
      } else if (current_pump_data.mode != pump_MachineMode_POWER_OFF) {
        update_and_send_power_status(
            static_cast<uint32_t>(PowerStatus::POWER_VOLTAGE_LOW), 0.0f);
      }

      previous_automate_auto_signal = automate_mode_signal;
      previousMode = current_pump_data.mode;
      log_voltage_low = !log_voltage_low;
    }

    return;
  }

  unsigned long currentTimeMs = getCurrentTimeMs();
  unsigned long signalTimeMs = currentTimeMs - lastChangeTimeMs;

  switch (current_pump_data.mode) {
  case pump_MachineMode_POWER_OFF:
    if (power.key == PowerStatus::POWER_RESTING &&
        signalTimeMs > current_pump_data.time_range.resting) {
      pumpState = true;
      powerOn = !pumpState;
    } else
      powerOn = false;
    break;
  case pump_MachineMode_AUTO:
    if (automate_mode_signal) {
      running_or_resting_and_update(signalTimeMs, currentTimeMs);
    } else if (power.key != PowerStatus::POWER_INACTIVE) {
      pumpState = true;
      powerOn = !pumpState;
    }
    break;
  case pump_MachineMode_POWER_ON:
    running_or_resting_and_update(signalTimeMs, currentTimeMs);
    break;

  default:
    LOG_LN("Invalid mode");
    break;
  }

  if (!log_voltage_low) {
    log_voltage_low = !log_voltage_low;
  }

  switch_pump(powerOn);
#ifndef PRODUCTION
  logPumpRunningState();
#endif
}

// Task to run the machine
void runMachineTask(void *parameter) {
  (void)parameter;
  pinMode(FLOAT_SIGNAL_PIN, INPUT_PULLUP);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  init_EEPROM_pump_controller();
  init_pzem_data();

  for (;;) {
    uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (notificationValue > 0) {
      controlPumpState();
    }
  }
}

void checkSignalTask(void *parameter) {

  (void)parameter;
  pinMode(GPIO_NUM_26, INPUT_PULLUP);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, ESP_EXT1_WAKEUP_ALL_LOW);

  for (;;) {
    int signal = 0;

    // Read the signal multiple times for debouncing
    for (int i = 0; i < SIGNAL_READ_COUNT; i++) {
      signal += digitalRead(FLOAT_SIGNAL_PIN) == LOW ? 1 : 0;
      vTaskDelay(pdMS_TO_TICKS(200));
    }

#ifdef FAKE_VOLTAGE_READING
    automate_mode_signal = test_auto_mode;
#else
    automate_mode_signal = signal >= 3; // 3 or more LOW readings to trigger
#endif

#ifndef PRODUCTION
    logPumpMode();
    logAutoSignal();
#endif

    if (digitalRead(GPIO_NUM_26) == HIGH) {
      LOG_LN("Going to sleep...");

      vTaskDelay(pdMS_TO_TICKS(SIGNAL_READ_DELAY_MS));

      if (digitalRead(LED_BUILTIN) == HIGH)
        digitalWrite(LED_BUILTIN, LOW);

      esp_light_sleep_start();

      LOG_LN("Woke up from light sleep");

      restartWifiAP();

      vTaskDelay(pdMS_TO_TICKS(SIGNAL_READ_DELAY_MS));
    }

    // Delay before next check
    vTaskDelay(pdMS_TO_TICKS(SIGNAL_READ_DELAY_MS));
  }
}
