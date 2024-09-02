#include "sensors.h"
#include "main.h"
#include "network.h"
#include "pump_control.h"
#include "type_id.h"
#include <EEPROM.h>
#include <str_num_msg_transcode.h>

TaskHandle_t sendVoltageTask = NULL;

const uint8_t adcPin = 34;
const float VREF = 3.3;
const int ADC_MAX = 4095;
const float SCALE_FACTOR = 11.0;

float min_voltage = 0.0f;

float readVoltage() {
#ifdef FAKE_VOLTAGE_READING
  return random(209, 215);
#endif

  int adcValue = analogRead(adcPin);
  float adcVoltage = adcValue * VREF / ADC_MAX;

  return (adcVoltage * SCALE_FACTOR);
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

void send_voltage_task(void *pvParameter) {
  (void)pvParameter;
  Num msg = Num_init_zero;
  msg.key = VoltageKey::VOLTAGE;
  pinMode(adcPin, INPUT);

  get_min_voltage(min_voltage);

  for (;;) {
    ws.cleanupClients();

    if (ws.count() == 0) {
      LOG_LN("Suspending sendVoltageTask");
      vTaskSuspend(NULL);
    }

    // Update reading_voltage before sending

    msg.value = readVoltage();

    send_num_message(msg, NUM_TYPE_ID);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}