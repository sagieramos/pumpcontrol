#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <esp_system.h>

#define DEBUG_SERIAL_ENABLED // Comment or uncomment this line to toggle serial
                             // output

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

const byte DNS_PORT = 53;

// Function prototypes
void setupWifiAP();
// void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Function prototype for blink task
void taskBlink(void *pvParameters);
void dnsTask(void *pvParameters);
void taskRun(void *pvParameters);

// Task handle for the blink task
extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t dnsTaskHandle;
extern const byte DNS_PORT;
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
