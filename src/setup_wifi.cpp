#include "main.h"
#include <WiFi.h>

// Define the custom IP configuration
IPAddress local_IP(192, 168, 10, 1); // Desired IP address
IPAddress gateway(192, 168, 10,
                  1); // Gateway, typically the same as local_IP for AP mode
IPAddress subnet(255, 255, 255, 0); // Subnet mask

uint8_t numStations = 0;

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_SERIAL_PRINTLN("............................................");

  switch (event) {
  case ARDUINO_EVENT_WIFI_AP_START:
    DEBUG_SERIAL_PRINTLN("SoftAP started");
    if (xTaskCreate(dnsTask, "DNS", 4086, NULL, 2, &dnsTaskHandle) !=
        pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to create DNS Task");
    } else {
      DEBUG_SERIAL_PRINTLN("DNS Task created");
      vTaskSuspend(dnsTaskHandle); // Suspend task initially
      DEBUG_SERIAL_PRINTLN("DNS Task suspended initially");
    }
    if (xTaskCreatePinnedToCore(taskBlink, "Blink", 2048, NULL, 3,
                                &blinkTaskHandle, 0) != pdPASS) {
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
  // Set up the access point IP settings
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    DEBUG_SERIAL_PRINTLN("Failed to configure SoftAP IP");
    return;
  }
  // Set up the access point
  if (!WiFi.softAP("Akowe_Fountain", "okosodo88")) {
    DEBUG_SERIAL_PRINTLN("Failed to start AP");
    return;
  }
  DEBUG_SERIAL_PRINTLN("Access Point started");
  DEBUG_SERIAL_PRINT("AP IP address: ");
  DEBUG_SERIAL_PRINTLN(WiFi.softAPIP());
}
