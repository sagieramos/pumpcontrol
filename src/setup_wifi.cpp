#include "dev_or_prod.h"
#include "main.h"
#include "network.h"
#include "sensors.h"
#include <WiFi.h>

// Define the custom IP configuration
IPAddress local_IP(192, 168, 10, 100); // Desired IP address
IPAddress gateway(192, 168, 10,
                  100); // Gateway, typically the same as local_IP for AP mode
IPAddress subnet(255, 255, 255, 0); // Subnet mask

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  LOG_LN("............................");
  uint8_t numStations = WiFi.softAPgetStationNum();

  switch (event) {
  case ARDUINO_EVENT_WIFI_AP_START:
    LOG_LN("SoftAP started");
    break;

  case ARDUINO_EVENT_WIFI_AP_STOP:
    LOG_LN("SoftAP stopped");
    digitalWrite(LED_PIN, LOW);
    break;

  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
    numStations = WiFi.softAPgetStationNum();
    LOG_F("Stations connected - AP: %d\n", numStations);
    break;

  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    numStations = WiFi.softAPgetStationNum();
    LOG_F("Stations disconnected - AP: %d\n", numStations);
    break;

  default:
    LOG_F("[WiFi-event] event: %d\n", event);
    numStations = WiFi.softAPgetStationNum();
    break;
  }

  check_and_resume_task(dnsTaskHandle, numStations > 0);
  check_and_resume_task(blinkTaskHandle, numStations < 1);
  ws.cleanupClients();
  if (numStations > 0) {
    digitalWrite(LED_PIN, HIGH);
  }
}

void setupWifiAP() {
  // Attach WiFi event handler
  WiFi.onEvent(WiFiEvent);
  // Set up the access point IP settings
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    LOG_LN("Failed to configure SoftAP IP");
    return;
  }
  // Set up the access point
  if (!WiFi.softAP("Akowe_Fountain", "okosodo88")) {
    LOG_LN("Failed to start AP");
    return;
  }
  LOG_LN("Access Point started");
  LOG_("AP IP address: ");
  LOG_LN(WiFi.softAPIP());
}

void restartWifiAP() {
  WiFi.softAPdisconnect(true);
  setupWifiAP();
}