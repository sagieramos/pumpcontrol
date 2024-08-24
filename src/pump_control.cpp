#include "pump_control.h"
#include "network.h"
#include "sensors.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t runMachineTask = NULL;
SemaphoreHandle_t controlDataMutex = NULL;

bool pumpState = false;
TimerHandle_t delayTimer = NULL;

// Get the current control data
pump_ControlData &get_current_control_data() {
  static pump_ControlData control_data = pump_ControlData_init_default;
  return control_data;
}

// Store time range in EEPROM
void store_time_range(bool check_changed) {
  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) != pdTRUE) {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in store_time_range()");
    return;
  }

  pump_ControlData &control_data = get_current_control_data();
  EEPROM.begin(EEPROM_SIZE_CTL);

  bool time_range_changed = true; // Assume change by default

  if (check_changed) {
    pump_TimeRange stored_time_range;
    EEPROM.get(MAGIC_NUMBER_SIZE, stored_time_range);

    time_range_changed =
        (stored_time_range.running != control_data.time_range.running) ||
        (stored_time_range.resting != control_data.time_range.resting);
  }

  if (time_range_changed) {
    DEBUG_SERIAL_PRINTF(
        "Storing Time Range - Running: %ld seconds\tResting: %ld seconds\n",
        control_data.time_range.running, control_data.time_range.resting);

    EEPROM.put(MAGIC_NUMBER_SIZE, control_data.time_range);
    EEPROM.commit();
    DEBUG_SERIAL_PRINTLN("Time Range updated in EEPROM");
  } else {
    DEBUG_SERIAL_PRINTLN("Time Range is unchanged, not updating EEPROM");
  }

  EEPROM.end();
  xSemaphoreGive(controlDataMutex);
}

// Validate if the time range is within acceptable limits (in seconds)
bool is_valid_time_range(const pump_TimeRange &time_range) {
  const long min_time = 300;           // 5 minutes in seconds
  const long max_time_running = 7200;  // 2 hours in seconds
  const long max_time_resting = 86400; // 24 hours in seconds

  return (time_range.running >= min_time &&
          time_range.running <= max_time_running &&
          time_range.resting >= min_time &&
          time_range.resting <= max_time_resting);
}

// Initialize EEPROM with pump controller settings
void init_EEPROM_pump_controller() {
  // Create the semaphore
  controlDataMutex = xSemaphoreCreateMutex();
  if (controlDataMutex == NULL) {
    DEBUG_SERIAL_PRINTLN("Failed to create semaphore");
    esp_deep_sleep_start();
  }

  // Attempt to take the semaphore
  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &control_data = get_current_control_data();

    EEPROM.begin(EEPROM_SIZE_CTL);

    uint8_t readMagicNumber;
    EEPROM.get(0, readMagicNumber);

    pump_TimeRange pump_time_range = pump_TimeRange_init_default;

    if (readMagicNumber == MAGIC_NUMBER) {
      EEPROM.get(MAGIC_NUMBER_SIZE, pump_time_range);
      DEBUG_SERIAL_PRINTF("Magic number found: %d\n", readMagicNumber);
      DEBUG_SERIAL_PRINTF(
          "Read Time Range - Running: %ld seconds\tResting: %ld seconds\n",
          pump_time_range.running, pump_time_range.resting);

      // Validate and set time range
      if (is_valid_time_range(pump_time_range)) {
        control_data.time_range = pump_time_range;
      } else {
        DEBUG_SERIAL_PRINTLN("Invalid time range, using default values");
        control_data.time_range = pump_TimeRange_init_default;
      }
    } else {
      DEBUG_SERIAL_PRINTF("Magic number not found: %d\n", readMagicNumber);

      // Store default values and magic number
      EEPROM.put(0, MAGIC_NUMBER);
      EEPROM.put(MAGIC_NUMBER_SIZE, pump_time_range);
      EEPROM.commit();
    }

    EEPROM.end();
    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in init_EEPROM_pump_controller()");
  }
}

// Control pump state based on mode and signal
void controlPumpState(bool trigger_auto_pump) {
  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &ctrl = get_current_control_data();
    unsigned long currentTime = getCurrentTimeMs();
    static unsigned long lastChangeTime = 0;
    static bool powerOn = false;

    if (ctrl.mode == pump_MachineMode_POWER_OFF) {
      if (!ctrl.is_running) {
        powerOn = false;
        ctrl.is_running = powerOn;
        xSemaphoreGive(controlDataMutex);
        switch_pump(powerOn);
      }
      return;
    }

    unsigned long signalTime = currentTime - lastChangeTime;
    signalTime /= 1000; // Convert milliseconds to seconds for comparison
    bool isVoltageSufficient = readVoltage() > min_voltage;

    switch (ctrl.mode) {
    case pump_MachineMode_AUTO:
      if (trigger_auto_pump && isVoltageSufficient) {
        if (signalTime >= ctrl.time_range.resting && !ctrl.is_running) {
          powerOn = true;
          lastChangeTime = currentTime;
        } else if (signalTime >= ctrl.time_range.running && ctrl.is_running) {
          powerOn = false;
          lastChangeTime = currentTime;
        }
      } else {
        powerOn = false;
      }
      break;

    case pump_MachineMode_POWER_ON:
      if (isVoltageSufficient) {
        if (signalTime >= ctrl.time_range.resting && !ctrl.is_running) {
          powerOn = true;
          lastChangeTime = currentTime;
        } else if (signalTime >= ctrl.time_range.running && ctrl.is_running) {
          powerOn = false;
          lastChangeTime = currentTime;
        }
      }
      break;

    default:
      DEBUG_SERIAL_PRINTLN("Invalid mode");
      break;
    }

    ctrl.is_running = powerOn;
    xSemaphoreGive(controlDataMutex);
    switch_pump(powerOn);
  }
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
