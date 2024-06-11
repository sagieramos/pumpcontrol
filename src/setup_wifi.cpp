#include "main.h"
#include <WiFi.h>

uint8_t numStations = 0;

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_SERIAL_PRINTLN("............................................");

  switch (event) {
  case ARDUINO_EVENT_WIFI_AP_START:
    DEBUG_SERIAL_PRINTLN("SoftAP started");
    if (xTaskCreate(dnsTask, "DNS Task", 2048, NULL, 1, &dnsTaskHandle) !=
        pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to create DNS Task");
    } else {
      DEBUG_SERIAL_PRINTLN("DNS Task created");
      vTaskSuspend(dnsTaskHandle); // Suspend task initially
      DEBUG_SERIAL_PRINTLN("DNS Task suspended initially");
    }
    if (xTaskCreatePinnedToCore(taskBlink, "Blink Task", 2048, NULL, 1,
                                &blinkTaskHandle, 1) != pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to create Blink Task");
    } else {
      // vTaskSuspend(blinkTaskHandle); // Suspend task initially
      DEBUG_SERIAL_PRINTLN("Blink Task created on core 1");
    }
    break;

  case ARDUINO_EVENT_WIFI_AP_STOP:
    DEBUG_SERIAL_PRINTLN("SoftAP stopped");
    // esp_restart();
    digitalWrite(LED_PIN, LOW);
    break;

  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
    numStations = WiFi.softAPgetStationNum();
    DEBUG_SERIAL_PRINTF("Stations connected - AP: %d\n", numStations);
    if (numStations > 0) {
      if (blinkTaskHandle != NULL) {
        vTaskSuspend(blinkTaskHandle);
        DEBUG_SERIAL_PRINTLN("Blink Task suspended");
      }
      if (dnsTaskHandle != NULL) {
        vTaskResume(dnsTaskHandle);
        DEBUG_SERIAL_PRINTLN("DNS Task resumed");
      }
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      if (blinkTaskHandle != NULL) {
        vTaskResume(blinkTaskHandle);
        DEBUG_SERIAL_PRINTLN("Blink Task resumed");
      }
      if (dnsTaskHandle != NULL) {
        vTaskSuspend(dnsTaskHandle);
        DEBUG_SERIAL_PRINTLN("DNS Task suspended");
      }
    }
    break;

  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    numStations = WiFi.softAPgetStationNum();
    DEBUG_SERIAL_PRINTF("Stations disconnected - AP: %d\n", numStations);
    if (numStations > 0) {
      if (blinkTaskHandle != NULL) {
        vTaskSuspend(blinkTaskHandle);
        DEBUG_SERIAL_PRINTLN("Blink Task suspended");
      }
      if (dnsTaskHandle != NULL) {
        vTaskResume(dnsTaskHandle);
      }
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      if (blinkTaskHandle != NULL) {
        vTaskResume(blinkTaskHandle);
        DEBUG_SERIAL_PRINTLN("Blink Task resumed");
      }
      if (dnsTaskHandle != NULL) {
        vTaskSuspend(dnsTaskHandle);
        DEBUG_SERIAL_PRINTLN("DNS Task suspended");
      }
    }
    break;

  default:
    DEBUG_SERIAL_PRINTF("[WiFi-event] event: %d\n", event);
    break;
  }
}

void setupWifiAP() {
  WiFi.disconnect(true);
  // Attach WiFi event handler
  WiFi.onEvent(WiFiEvent);
  // Set up the access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Akowe_Fountain", "okosodo88");
  DEBUG_SERIAL_PRINTLN("Access Point started");
}
