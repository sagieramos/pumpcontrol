#include "main.h"

uint8_t numStations = 0;

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
  case 10: // SYSTEM_EVENT_AP_START
    if (xTaskCreate(dnsTask, "DNS Task", 2048, NULL, 1, &dnsTaskHandle) !=
        pdPASS) {
      Serial.println("Failed to create DNS Task");
    } else {
      vTaskSuspend(dnsTaskHandle); // Suspend task initially
    }
    if (xTaskCreatePinnedToCore(taskBlink, "Blink Task", 2048, NULL, 1,
                                &blinkTaskHandle, 1) != pdPASS) {
      Serial.println("Failed to create Blink Task");
    } else {
      vTaskSuspend(blinkTaskHandle); // Suspend task initially
      digitalWrite(LED_PIN, LOW);
    }
    Serial.println("SoftAP started");
    break;

  case 11: // SYSTEM_EVENT_AP_STOP:
    if (dnsTaskHandle != NULL) {
      vTaskDelete(dnsTaskHandle);
      dnsTaskHandle = NULL;
    }
    if (blinkTaskHandle != NULL) {
      vTaskDelete(blinkTaskHandle);
      blinkTaskHandle = NULL;
    }
    Serial.println("SoftAP stopped");
    break;

  case 12: // SYSTEM_EVENT_AP_STACONNECTED:
  case 13: // SYSTEM_EVENT_AP_STADISCONNECTED:
    numStations = WiFi.softAPgetStationNum();
    Serial.printf("Number of stations connected to the AP: %d\n", numStations);
    digitalWrite(LED_PIN, numStations > 0 ? HIGH : LOW);
    if (numStations > 0) {
      Serial.println("Station connected");
      if (blinkTaskHandle != NULL) {
        vTaskResume(blinkTaskHandle);
      }
      if (dnsTaskHandle != NULL) {
        vTaskResume(dnsTaskHandle);
      }
    } else {
      Serial.println("Station disconnected");
      if (blinkTaskHandle != NULL) {
        vTaskSuspend(blinkTaskHandle);
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

/* void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface
ready"); break; case ARDUINO_EVENT_WIFI_SCAN_DONE: Serial.println("Completed
scan for access points"); break; case ARDUINO_EVENT_WIFI_STA_START:
Serial.println("WiFi client started"); break; case ARDUINO_EVENT_WIFI_STA_STOP:
Serial.println("WiFi clients stopped"); break; case
ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access
point"); break; case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
Serial.println("Disconnected from WiFi access point"); break; case
ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of
access point has changed"); break; case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address
and IP address is reset to 0"); break; case ARDUINO_EVENT_WPS_ER_SUCCESS:
Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected
Setup (WPS): failed in enrollee mode"); break; case
ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup
(WPS): timeout in enrollee mode"); break; case ARDUINO_EVENT_WPS_ER_PIN:
Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access
point started"); break; case ARDUINO_EVENT_WIFI_AP_STOP: Serial.println("WiFi
access point  stopped"); break; case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
Serial.println("Client connected"); break; case
ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected");
break; case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("Assigned IP
address to client"); break; case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
Serial.println("Received probe request"); break; case
ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred");
break; case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        Serial.println("STA IPv6 is
preferred"); break; case ARDUINO_EVENT_ETH_GOT_IP6: Serial.println("Ethernet
IPv6 is preferred"); break; case ARDUINO_EVENT_ETH_START:
Serial.println("Ethernet started"); break; case ARDUINO_EVENT_ETH_STOP:
Serial.println("Ethernet stopped"); break; case ARDUINO_EVENT_ETH_CONNECTED:
Serial.println("Ethernet connected"); break; case
ARDUINO_EVENT_ETH_DISCONNECTED:        Serial.println("Ethernet disconnected");
break; case ARDUINO_EVENT_ETH_GOT_IP:              Serial.println("Obtained IP
address"); break; default:                                    break;
  }
} */

void setupWifiAP() {
  WiFi.disconnect(true);
  // Attach WiFi event handler
  WiFi.onEvent(WiFiEvent);
  // Set up the access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Akowe_Fountain", "okosodo88");
  Serial.println("Access Point started");
}
