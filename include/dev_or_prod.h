#ifndef DEV_OR_PROD_H
#define DEV_OR_PROD_H

#ifndef TESTING
#include <Arduino.h>

#ifndef PRODUCTION
#define DEBUG_SERIAL_ENABLED
#endif

#endif // TESTING

#ifdef DEBUG_SERIAL_ENABLED
#define DEBUG_SERIAL_BEGIN(baud) Serial.begin(baud)
#define DEBUG_SERIAL_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_SERIAL_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_SERIAL_BEGIN(baud) ((void)0)
#define DEBUG_SERIAL_PRINT(...) ((void)0)
#define DEBUG_SERIAL_PRINTLN(...) ((void)0)
#define DEBUG_SERIAL_PRINTF(...) ((void)0)
#endif

unsigned int getCurrentTimeMs();
bool check_and_resume_task(TaskHandle_t task);

#endif // DEV_OR_PROD_H
