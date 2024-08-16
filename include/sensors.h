#ifndef SENSOR_H
#define SENSOR_H

#include "dev_or_prod.h"

#define DEFAULT_MIN_VOLTAGE 200.0f

extern TaskHandle_t sendVoltageTask;
extern float min_voltage;

void readVoltage(float &voltage);
bool store_min_voltage(float min_voltage);
void send_voltage_task(void *pvParameter);

#endif