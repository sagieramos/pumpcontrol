#ifndef DEV_OR_PROD_H
#define DEV_OR_PROD_H

#ifndef TESTING
#include <Arduino.h>

#ifndef PRODUCTION
#define LOG_ENABLED
#endif

#endif // TESTING

#ifdef LOG_ENABLED
#define LOG_BEGIN(baud) Serial.begin(baud)
#define LOG_(...) Serial.print(__VA_ARGS__)
#define LOG_LN(...) Serial.println(__VA_ARGS__)
#define LOG_F(...) Serial.printf(__VA_ARGS__)
#else
#define LOG_BEGIN(baud) ((void)0)
#define LOG_(...) ((void)0)
#define LOG_LN(...) ((void)0)
#define LOG_F(...) ((void)0)
#endif

unsigned long getCurrentTimeMs();
void check_and_resume_task(TaskHandle_t task, bool resumeTask);

#endif // DEV_OR_PROD_H
