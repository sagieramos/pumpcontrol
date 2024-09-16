#include "sensors.h"
#include "main.h"
#include "pump_control.h"
#include <str_num_msg_transcode.h>

TaskHandle_t sendVoltageTask = NULL;

PZEM004Tv30 pzem(Serial2, 16, 17);

float min_voltage = 0.0f;

/*
 * Voltage reading from PZEM004Tv30
 * Voltage reading from PZEM004Tv30 is stored in Msg1::f0
 * Power reading from PZEM004Tv30 is stored in Msg1::f1
 * Power factor reading from PZEM004Tv30 is stored in Msg1::f2
 */
Msg1 pzemTwoData = Msg1_init_zero;

Num msg = {VoltageKey::MIN_VOLTAGE, 0};

float readVoltage() {
#ifdef FAKE_VOLTAGE_READING
  return random(209, 215);
#endif
  float voltage = pzem.voltage();

  /*   LOG_F("Voltage: %f\n", voltage); */
  return (isnan(voltage) ? 0.0f : voltage);
}

// Store minimum voltage to power the pump in EEPROM
bool store_min_voltage(float voltage) {
  if (voltage < 110.0f || voltage > 240.0f) {
    LOG_F("Invalid min voltage: %f\n", voltage);
    LOG_LN("Min voltage should be between 110 and 240");
    return false;
  }

  min_voltage = voltage;
  LOG_F("Min voltage: %f\n", min_voltage);

  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.put(MIN_VOLTAGE_ADDRESS, voltage);
  EEPROM.commit();
  return true;
}

// Get the minimum voltage to power the pump
void get_min_voltage(float &voltage) {
  float volt = 0.0f;
  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.get(MIN_VOLTAGE_ADDRESS, volt);
  if (volt < 110.0f || volt > 250.0f || isnan(volt)) {
    LOG_LN("Invalid min voltage");
    LOG_LN("Setting default min voltage to 200");
    volt = DEFAULT_MIN_VOLTAGE;
  }

  // generate random number between 209 and 213
  voltage = volt;
  LOG_F("Min voltage: %f\n", voltage);
}

void send_min_voltage(AsyncWebSocketClient *client) {
  msg.value = min_voltage;
  uint8_t buffer[Num_size];
  size_t buffer_size = 0;
  if (!serialize_num(msg, buffer, &buffer_size, NUM_TYPE_ID)) {
    LOG_LN("Failed to serialize min voltage message");
  } else {
    if (client) {
      client->binary(buffer, buffer_size);
    } else {
      send_binary_data(buffer, buffer_size);
    }
  }
  LOG_F("Sent min voltage: %f | key: %d | typeId: %d\n", msg.value, msg.key,
        NUM_TYPE_ID);
}

static void send_pzemTwo_data() {
  uint8_t buffer[Msg1_size];
  size_t buffer_size = 0;

  if (!serialize_msg1(pzemTwoData, buffer, &buffer_size, PZEM2_TYPE_ID)) {
    LOG_LN("Failed to serialize pzemTwo data message");
  } else {
    ws.binaryAll(buffer, buffer_size);

    LOG_F("Sent pzemTwoData voltage(f0): %f | power(f1): %f | power "
          "factor(f2): %f | typeId: %d | Buffer size: %d\n",
          pzemTwoData.f0, pzemTwoData.f1, pzemTwoData.f2, PZEM2_TYPE_ID,
          buffer_size);
  }
}

/* static void send_voltage_data() {
  Num msg = {VoltageKey::VOLTAGE, 0.0f};
  msg.value = pzemTwoData.f0;

  uint8_t buffer[Num_size];
  size_t buffer_size = 0;
  if (!serialize_num(msg, buffer, &buffer_size, PZEM2_TYPE_ID)) {
    LOG_LN("Failed to serialize voltage message");
  } else {
    ws.binaryAll(buffer, buffer_size);

    LOG_F("Sent pzemTwoData voltage(f0): %f | power(f1): %f | power "
          "factor(f2): %f | typeId: %d | Buffer size: %d\n",
          pzemTwoData.f0, pzemTwoData.f1, pzemTwoData.f2, PZEM2_TYPE_ID,
          buffer_size);
  }
} */

void reset_energy() {
  if (pzem.resetEnergy()) {
    LOG_LN("Energy reset successful");
  } else {
    LOG_LN("Energy reset failed");
  }
}

void send_voltage_task(void *pvParameter) {
  (void)pvParameter;

  Num msg = {VoltageKey::VOLTAGE, 0.0f};

  get_min_voltage(min_voltage);

  for (;;) {
    ws.cleanupClients();

    if (ws.count() == 0) {
      LOG_LN("No WebSocket clients connected, suspending sendVoltageTask.");
      vTaskSuspend(NULL);
    } else {
      if (!ws.availableForWriteAll()) {
        LOG_LN("WebSocket write not available, suspending sendVoltageTask.");
        vTaskSuspend(NULL);
      }

      send_pzemTwo_data();

      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

void readPzemTask(void *parameter) {
  (void)parameter;
  for (;;) {
    pzemTwoData.f0 = readVoltage();

    if (!current_pump_data.is_running) {
      pzemTwoData.f1 = 0.0f;
      pzemTwoData.f2 = 0.0f;
    } else {
      pzemTwoData.f1 = pzem.power();
      pzemTwoData.f2 = pzem.pf();
    }
    xTaskNotifyGive(runMachineTaskHandle);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}