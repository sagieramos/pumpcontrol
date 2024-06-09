#include "main.h"

uint8_t numStations = 0;

enum WiFiEvent_t {
  SYSTEM_EVENT_AP_START = 10,
  SYSTEM_EVENT_AP_STOP,
  SYSTEM_EVENT_AP_STACONNECTED,
  SYSTEM_EVENT_AP_STADISCONNECTED
};

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
  case SYSTEM_EVENT_AP_START:
    if (xTaskCreate(dnsTask, "DNS Task", 2048, NULL, 1, &dnsTaskHandle) != pdPASS) {
      Serial.println("Failed to create DNS Task");
    } else {
      vTaskSuspend(dnsTaskHandle); // Suspend task initially
    }
    if (xTaskCreatePinnedToCore(taskBlink, "Blink Task", 2048, NULL, 1, &blinkTaskHandle, 1) != pdPASS) {
      Serial.println("Failed to create Blink Task");
    } else {
      vTaskSuspend(blinkTaskHandle); // Suspend task initially
      digitalWrite(LED_PIN, LOW);
    }
    Serial.println("SoftAP started");
    break;

  case SYSTEM_EVENT_AP_STOP:
    Serial.println("SoftAP stopped");
    esp_restart();
    break;

  case SYSTEM_EVENT_AP_STACONNECTED:
  case SYSTEM_EVENT_AP_STADISCONNECTED:
    numStations = WiFi.softAPgetStationNum();
    Serial.printf("Number of stations connected to the AP: %d\n", numStations);
    digitalWrite(LED_PIN, HIGH);
    if (numStations > 0) {
      Serial.println("Station connected");
      if (blinkTaskHandle != NULL) {
        vTaskSuspend(blinkTaskHandle);
      }
      if (dnsTaskHandle != NULL) {
        vTaskResume(dnsTaskHandle);
      }
    } else {
      Serial.println("Station disconnected");
      if (blinkTaskHandle != NULL) {
        vTaskResume(blinkTaskHandle);
      }
      if (dnsTaskHandle != NULL) {
        vTaskSuspend(dnsTaskHandle);
      }
    }
    break;

  default:
    Serial.printf("[WiFi-event] event: %d\n", event);
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
  Serial.println("Access Point started");
}
