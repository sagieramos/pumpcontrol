#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include "dev_or_prod.h"
#include "msg_type_identifier.h"
#include "network.h"
#include <transcode_pump_control.h>

constexpr uint32_t TIME_ON(40 * 60000);
constexpr uint32_t TIME_OFF(20 * 60000);

constexpr int FLOAT_SIGNAL_PIN = 21;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr size_t EEPROM_SIZE_CTL = sizeof(pump_TimeRange) + MAGIC_NUMBER_SIZE;

extern TaskHandle_t runMachineTask;

void send_control_data(const size_t client_id);
void store_pump_time_range(const pump_TimeRange *time_range);
void store_pump_time_range();
void setup_pump_controller();

#endif // PUMP_CONTROL_H