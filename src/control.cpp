#include "domsgid.h"
#include "main.h"
#include "network.h"
#include <EEPROM.h>

constexpr int FLOAT_SIGNAL_PIN = 21;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr int FLOAR_SIGNAL_TRESHOLD = 1500;
constexpr size_t EEPROM_SIZE_CTL = sizeof(control) + MAGIC_NUMBER_SIZE;

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

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static bool fields_encode(pb_ostream_t *stream, const pb_field_t *field,
                          void *const *arg) {
  DoId message = DoId_init_zero;
  const controlData &ctrl = getControlData();
  const unsigned int obj[3] = {static_cast<unsigned int>(ctrl.mode),
                               ctrl.timer.running, ctrl.timer.resting};

  for (size_t i = 0; i < 3; ++i) {
    message.id = MAGIC_NUMBER + i;
    message.value = obj[i];

    if (!pb_encode_tag_for_field(stream, field)) {
      return false;
    }

    if (!pb_encode_submessage(stream, DoId_fields, &message)) {
      return false;
    }
  }

  return true;
}

bool fields_decode(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  DoId message = DoId_init_zero;
  controlData &ctrl = getControlData();

  if (!pb_decode(stream, DoId_fields, &message)) {
    return false;
  }

  DEBUG_SERIAL_PRINTF("DoId - ID: %d\tValue: %d\n", message.id, message.value);

  return true;
}

bool send_ctr_data(const size_t client_id) {
  if (client_id == 0) {
    return false;
  }
  unsigned char buffer[1024];

  DoIdList message = DoIdList_init_zero;
  message.doid.funcs.encode = fields_encode;
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool status = pb_encode(&ostream, DoIdList_fields, &message);

  if (status) {
    for (size_t i = 0; i < ostream.bytes_written; i++) {
      DEBUG_SERIAL_PRINTF("%02X ", buffer[i]);
    }

    DEBUG_SERIAL_PRINTLN();
  }

  if (status && client_id == 0) {
    // send to all clients
    ws.binaryAll(buffer, ostream.bytes_written);
    return true;
  } else if (status) {
    // send to specific client
    ws.binary(client_id, buffer, ostream.bytes_written);
    return true;
  } else {
    return false;
  }
}

void setupPumpControl() {
  pinMode(FLOAT_SIGNAL_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  EEPROM.begin(
      EEPROM_SIZE_CTL); // Initialize EEPROM with size. already initialized in
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
