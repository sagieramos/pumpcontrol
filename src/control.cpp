#include "main.h"

constexpr int FLOAT_SIGNAL_PIN = 13;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
bool isControlDataPresent = false; // Flag to check if control data is present
bool signalState = false;          // Flag to track signal state

controlData &getControlData() {
  static controlData ctrl;
  static bool checkEEPROMForData = true;
  // Read control from EEPROM
  if (checkEEPROMForData) {
    EEPROM.get(sizeof(MAGIC_NUMBER), ctrl.timer);
    checkEEPROMForData = false;
  }

  return ctrl;
}

bool setControlData(const control &timer) {
  // Write control to EEPROM
  static bool writeSignatureOnce = true;

  if (writeSignatureOnce) {
    EEPROM.put(0, MAGIC_NUMBER);
    writeSignatureOnce = false;
  }

  EEPROM.put(sizeof(MAGIC_NUMBER), timer);
  return EEPROM.commit();
}

void controlPump(bool state) {
  static bool pumpState = false; // Static variable to track current pump state

  if (state !=
      pumpState) {     // Only change state if it's different from current state
    pumpState = state; // Update the tracked state
    digitalWrite(PUMP_RELAY_PIN,
                 state ? HIGH : LOW); // Turn Pump ON or OFF based on state
    DEBUG_SERIAL_PRINTLN(state ? "Pump is ON" : "Pump is OFF");
  }
}

void controlPumpState() {
  controlData &ctrl = getControlData();
  switch (ctrl.mode) {
  case MachineMode::POWER_ON:
    controlPump(true);
    break;
  case MachineMode::POWER_OFF:
    controlPump(false);
    break;
  case MachineMode::AUTO: {
    static unsigned long lastChangeTime = 0;
    unsigned long currentTime = millis();

    if (signalState) {
      // Signal is high
      if (currentTime - lastChangeTime >= ctrl.timer.running) {
        controlPump(false); // Turn Pump OFF after running
        lastChangeTime = currentTime;
      } else {
        controlPump(true); // Keep Pump ON
      }
    } else {
      // Signal is low
      if (currentTime - lastChangeTime >= ctrl.timer.resting) {
        controlPump(true); // Turn Pump ON after timeOff
        lastChangeTime = currentTime;
      } else {
        controlPump(false); // Keep Pump OFF
      }
    }
  } break;
  default:
    DEBUG_SERIAL_PRINTLN("Invalid mode");
    break;
  }
}

void configPump(MachineMode mode, unsigned int running, unsigned int resting) {
  controlData &ctrl = getControlData();
  ctrl.timer.running = running;
  ctrl.timer.resting = resting;

  // Write control to EEPROM
  setControlData(ctrl.timer);
}

void configPump(MachineMode mode) {
  controlData &ctrl = getControlData();
  ctrl.mode = mode;
}

void configPump(unsigned int running, unsigned int resting) {
  controlData &ctrl = getControlData();
  ctrl.timer.running = running;
  ctrl.timer.resting = resting;

  // Write control to EEPROM
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

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    UBaseType_t stackHighwater = uxTaskGetStackHighWaterMark(NULL);
    DEBUG_SERIAL_PRINTF("Float Signal Task Stack Highwater: %u\n",
                        stackHighwater);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void runPumpControl() {
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  uint8_t magicNumber = 123;

  // Read magic number from EEPROM to check if control data is present
  EEPROM.get(0, magicNumber);
  isControlDataPresent = (magicNumber == MAGIC_NUMBER);

  xTaskCreate(runMachine, "Float Signal Task", 2048, NULL, 1, NULL);
}
