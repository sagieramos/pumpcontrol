#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>

// Function prototypes
void setup_wifi();
// void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Function prototype for blink task
void taskBlink(void *pvParameters);

void taskRun(void *pvParameters);

// Task handle for the blink task
extern TaskHandle_t blinkTaskHandle;
extern DNSServer dnsServer;

String getTaskInfo(TaskHandle_t taskHandle);

// Constants
/* const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *mqtt_server = "mqtt.example.com";
const int mqtt_port = 1883; */

// Macros
#define LED_PIN 2

#endif // MY_HEADER_H
