#ifndef SENSOR_H
#define SENSOR_H

#include "main.h"

#define DEFAULT_MIN_VOLTAGE 200.0f

extern TaskHandle_t sendVoltageTask;
extern TaskHandle_t readVoltageTask;

void readVoltage(float &voltage);
bool store_min_voltage(float min_voltage);
void read_voltage_task(void *pvParameter);
void send_voltage_task(void *pvParameter);

#endif