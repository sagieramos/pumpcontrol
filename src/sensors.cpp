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

void readVoltage(float &voltage) {
  int adcValue = analogRead(adcPin);
  float adcVoltage = adcValue * VREF / ADC_MAX;
  voltage = adcVoltage * SCALE_FACTOR;
#ifndef PRODUCTION
  voltage = random(209, 213);
#endif
}

// Store minimum voltage to power the pump in EEPROM
bool store_min_voltage(float voltage) {
  if (voltage < 110.0f || voltage > 240.0f) {
    DEBUG_SERIAL_PRINTF("Invalid min voltage: %f\n", voltage);
    DEBUG_SERIAL_PRINTLN("Min voltage should be between 110 and 240");
    return false;
  }
  min_voltage = voltage;
  DEBUG_SERIAL_PRINTF("Min voltage: %f\n", min_voltage);
  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.put(MAGIC_NUMBER_SIZE + sizeof(pump_TimeRange), voltage);
  EEPROM.commit();
  return true;
}

// Get the minimum voltage to power the pump
void get_min_voltage(float &voltage) {
  float volt = 0.0f;
  EEPROM.begin(EEPROM_SIZE_CTL);
  EEPROM.get(MAGIC_NUMBER_SIZE + sizeof(pump_TimeRange), volt);
  if (volt < 110.0f || volt > 250.0f || isnan(volt)) {
    DEBUG_SERIAL_PRINTLN("Invalid min voltage");
    DEBUG_SERIAL_PRINTLN("Setting default min voltage to 200");
    volt = DEFAULT_MIN_VOLTAGE;
  }

  // generate random number between 209 and 213
  voltage = volt;
  DEBUG_SERIAL_PRINTF("Min voltage: %f\n", voltage);
}

void send_voltage_task(void *pvParameter) {
  (void)pvParameter;
  Num msg = Num_init_zero;
  pinMode(adcPin, INPUT);

  static float reading_voltage = 0.0f;

  get_min_voltage(min_voltage);

  for (;;) {
    ws.cleanupClients();

    if (ws.count() == 0) {
      DEBUG_SERIAL_PRINTLN("Suspending sendVoltageTask");
      vTaskSuspend(NULL);
    }

    // Update reading_voltage before sending
    readVoltage(reading_voltage);

    msg.value = reading_voltage;

    send_num_message(msg, VOLTAGE_TYPE_ID);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}