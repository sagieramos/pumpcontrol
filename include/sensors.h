#ifndef SENSOR_H
#define SENSOR_H

#include "dev_or_prod.h"
#include <transcode_msg1.h>

#define DEFAULT_MIN_VOLTAGE 200.0f

extern TaskHandle_t sendVoltageTask;
extern float min_voltage;

extern Msg1 pzemTwoData;

float readVoltage();
bool store_min_voltage(float min_voltage);
void send_voltage_task(void *pvParameter);

#endif