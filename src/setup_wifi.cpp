#include "dev_or_prod.h"
#include "main.h"
#include "network.h"
#include "sensors.h"
#include <WiFi.h>

// Define the custom IP configuration
IPAddress local_IP(192, 168, 10, 1); // Desired IP address
IPAddress gateway(192, 168, 10,
                  1); // Gateway, typically the same as local_IP for AP mode
IPAddress subnet(255, 255, 255, 0); // Subnet mask

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_SERIAL_PRINTLN("............................................");
  uint8_t numStations = WiFi.softAPgetStationNum();

  switch (event) {
  case ARDUINO_EVENT_WIFI_AP_START:
    DEBUG_SERIAL_PRINTLN("SoftAP started");
    if (xTaskCreate(dnsTask, "DNS", 4086, NULL, 2, &dnsTaskHandle) != pdPASS) {
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
    break;

  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    numStations = WiFi.softAPgetStationNum();
    DEBUG_SERIAL_PRINTF("Stations disconnected - AP: %d\n", numStations);
    break;

  default:
    DEBUG_SERIAL_PRINTF("[WiFi-event] event: %d\n", event);
    numStations = WiFi.softAPgetStationNum();
    break;
  }

  check_and_resume_task(dnsTaskHandle, numStations > 0);
  check_and_resume_task(blinkTaskHandle, numStations < 1);
  check_and_resume_task(sendVoltageTask, numStations > 0);
  ws.cleanupClients();
  if (numStations > 0) {
    digitalWrite(LED_PIN, HIGH);
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
