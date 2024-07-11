#include "main.h"
#include <EEPROM.h>

constexpr int FLOAT_SIGNAL_PIN = 21;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr int FLOAR_SIGNAL_TRESHOLD = 1500;
bool signalState = false; // Flag to track signal state

struct machineObjStatus {
  bool machineIsOn = false;
  bool waterIsLow = false;
};

machineObjStatus machineStatus;

TaskHandle_t runMachineTask = NULL;

controlData &getControlData() {
  static controlData ctrl;
  return ctrl;
}

bool setControlData(const control &timer) {
  // Write control to EEPROM
  static control preTimer;

  if (preTimer.running == timer.running && preTimer.resting == timer.resting) {
    return true;
  }

  preTimer = timer;

  EEPROM.put(MAGIC_NUMBER_SIZE, timer);
  return EEPROM.commit();
}

bool controlPump(bool state) {
  static bool pumpState = false; // Static variable to track current pump state

  // Only change state if it's different from current state
  if (state != pumpState) {
    pumpState = state; // Update the tracked state
    // Turn Pump ON or OFF based on state
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
  controlData &ctrl = getControlData();
  switch (ctrl.mode) {
  case MachineMode::AUTO: {
    if (signalState) {
      static unsigned long lastChangeTime = 0;
      unsigned long currentTime = getCurrentTimeMs();
      unsigned long signalTime = currentTime - lastChangeTime;
      if (signalTime >= ctrl.timer.resting) {
        lastChangeTime = currentTime;
        // Turn Pump OFF after running
        if (controlPump(false)) {
          DEBUG_SERIAL_PRINTF("Pump ran in: %lums\n", signalTime);
        }
      } else if (signalTime >= ctrl.timer.running) {
        // Keep Pump ON after running
        if (controlPump(true)) {
          DEBUG_SERIAL_PRINTF("Pump ran in: %lums\n", signalTime);
        }
      }
    } else {
      controlPump(false);
    }
  } break;
  case MachineMode::POWER_ON:
    controlPump(true);
    break;
  case MachineMode::POWER_OFF:
    controlPump(false);
    break;
  default:
    DEBUG_SERIAL_PRINTLN("Invalid mode");
    break;
  }
}

void configPump(MachineMode mode, unsigned int runningTime,
                unsigned int restingTime) {
  controlData &ctrl = getControlData();
  ctrl.timer.running = runningTime;
  ctrl.timer.resting = restingTime;
  ctrl.mode = mode;

  // Write control to EEPROM
  setControlData(ctrl.timer);
}

void configPump(MachineMode mode) {
  controlData &ctrl = getControlData();
  ctrl.mode = mode;
}

void configPump(unsigned int runningTime, unsigned int restingTime) {
  controlData &ctrl = getControlData();
  ctrl.timer.running = runningTime;
  ctrl.timer.resting = restingTime;

  setControlData(ctrl.timer);
}

void runMachine(void *parameter) {
  (void)parameter;
  for (;;) {
    int signal = 0;

    for (int i = 0; i < 5; i++) {
      signal += digitalRead(FLOAT_SIGNAL_PIN) == HIGH ? 1 : 0;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    signalState = signal >= 4;
    controlPumpState();

    // UBaseType_t stackHighwater = uxTaskGetStackHighWaterMark(NULL);
    /*     DEBUG_SERIAL_PRINTF("runMachine() stack size: %u\n",
                            uxTaskGetStackHighWaterMark(NULL)); */

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setupPumpControl() {
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  // EEPROM.begin(512); // Initialize EEPROM with size. already initialized in
  // main.cpp

  if (EEPROM.read(0) != MAGIC_NUMBER) {
    control timer;
    EEPROM.put(0, MAGIC_NUMBER);
    EEPROM.put(MAGIC_NUMBER_SIZE, timer);
    EEPROM.commit();

    DEBUG_SERIAL_PRINTLN(
        "Control data not present. Writing default data to EEPROM");
  }

  controlData &ctrl = getControlData();

  EEPROM.get(MAGIC_NUMBER_SIZE, ctrl.timer);

  DEBUG_SERIAL_PRINT("Mode: ");
  DEBUG_SERIAL_PRINT(static_cast<int>(ctrl.mode));
  DEBUG_SERIAL_PRINTLN();

  DEBUG_SERIAL_PRINTLN("Control data read from EEPROM");

  DEBUG_SERIAL_PRINT("Running Time: ");
  DEBUG_SERIAL_PRINT(ctrl.timer.running);
  DEBUG_SERIAL_PRINTLN(" ms");

  DEBUG_SERIAL_PRINT("Resting Time: ");
  DEBUG_SERIAL_PRINT(ctrl.timer.resting);
  DEBUG_SERIAL_PRINTLN(" ms");

  xTaskCreate(runMachine, "Machine", 2560, NULL, 1, &runMachineTask);
}
