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
void store_time_range() {
  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &control_data = get_current_control_data();

    EEPROM.begin(EEPROM_SIZE_CTL);
    pump_TimeRange stored_time_range = pump_TimeRange_init_zero;
    EEPROM.get(MAGIC_NUMBER_SIZE, stored_time_range);

    bool time_range_changed =
        (stored_time_range.running != control_data.time_range.running) ||
        (stored_time_range.resting != control_data.time_range.resting);

    if (time_range_changed) {
      DEBUG_SERIAL_PRINTF("Storing Time Range - Running: %d\tResting: %d\n",
                          control_data.time_range.running,
                          control_data.time_range.resting);

      EEPROM.put(MAGIC_NUMBER_SIZE, control_data.time_range);
      EEPROM.commit();
      DEBUG_SERIAL_PRINTLN("Time Range updated in EEPROM");
    } else {
      DEBUG_SERIAL_PRINTLN("Time Range is unchanged, not updating EEPROM");
    }

    EEPROM.end();
    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in store_time_range()");
  }
}

// Initialize EEPROM with pump controller settings
void init_EEPROM_pump_controller() {
  controlDataMutex = xSemaphoreCreateMutex();
  if (controlDataMutex == NULL) {
    DEBUG_SERIAL_PRINTLN("Failed to create semaphore");
    esp_deep_sleep_start();
  }
  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
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
    static unsigned long lastChangeTime = 0;
    static bool powerOn = false;

    switch (ctrl.mode) {
    case pump_MachineMode_AUTO: {
      float voltageReading = 0.0f;
      readVoltage(voltageReading);
      if (trigger_auto_pump && voltageReading) {
        unsigned long currentTime = getCurrentTimeMs();
        unsigned long signalTime = currentTime - lastChangeTime;
        if (signalTime >= ctrl.time_range.resting) {
          lastChangeTime = currentTime;
          powerOn = false;
        } else if (signalTime >= ctrl.time_range.running) {
          powerOn = true;
        }
      } else {
        powerOn = false;
      }
      break;
    }

    case pump_MachineMode_POWER_ON:
      powerOn = true;
      break;

    case pump_MachineMode_POWER_OFF:
      powerOn = false;
      break;

    default:
      DEBUG_SERIAL_PRINTLN("Invalid mode");
      break;
    }
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
