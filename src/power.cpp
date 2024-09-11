#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

TaskHandle_t powerControlTask = NULL;

#define POWER_ON 1
#define POWER_OFF 0

constexpr unsigned long MS_TO_S = 1000;
constexpr unsigned long PUMP_DELAY_MS = 10000;

Num power = Num_init_default;

bool pumpState = false;

unsigned long last_change_time_ms = 0;

// Helper function to update and send power status
void update_and_send_power_status(uint32_t key, float value) {
  if (power.key == key && power.value == value) {
    return;
  }
  power.key = key;
  power.value = value;

  send_num_message(power, POWER_TYPE_ID);
}

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  xTaskNotify(powerControlTask, POWER_ON, eSetValueWithOverwrite);
}

// Start a timer to turn on the pump
void startPumpDelayTimer() {
  if (delayTimer == nullptr) {
    delayTimer = xTimerCreate("PumpDelayTimer", pdMS_TO_TICKS(PUMP_DELAY_MS),
                              pdFALSE, 0, power_on_cb);
    if (delayTimer == nullptr) {
      LOG_LN("Failed to create PumpDelayTimer");
      return;
    }
  }

  if (xTimerStart(delayTimer, 0) != pdPASS) {
    LOG_LN("Failed to start PumpDelayTimer");
  } else {
    LOG_LN("Pump delay timer started");
    LOG_F("Pump will be ON in %lu seconds\n", PUMP_DELAY_MS / MS_TO_S);

    last_change_time_ms = getCurrentTimeMs();

    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_READY),
        static_cast<float>(PUMP_DELAY_MS / MS_TO_S));
  }
}

// Stop and clean up the timer
void stopAndCleanupTimer() {
  if (delayTimer != nullptr) {
    if (xTimerStop(delayTimer, 0) != pdPASS) {
      LOG_LN("Failed to stop PumpDelayTimer");
    }
    if (xTimerDelete(delayTimer, 0) != pdPASS) {
      LOG_LN("Failed to delete PumpDelayTimer");
    }
    delayTimer = nullptr;
    LOG_LN("Pump delay timer stopped");
  }
}

// Switch pump state with proper handling
void switch_pump(bool state) {
  if (state == pumpState)
    return;

  LOG_F("Switching pump state to %s\n", state ? "ON" : "OFF");

  pumpState = state;

  if (pumpState) {
    startPumpDelayTimer();
  } else {
    stopAndCleanupTimer();
    xTaskNotify(powerControlTask, POWER_OFF, eSetValueWithOverwrite);
  }
}

// Function to send the current power status and remaining time to a WebSocket
// client
void send_all_power_status_and_type(AsyncWebSocketClient *client) {
  PowerStatus power_status = static_cast<PowerStatus>(power.key);
  unsigned long current_time_ms = getCurrentTimeMs();
  float adjusted_time = 0.0f;

  switch (power_status) {
  case POWER_READY:
    adjusted_time = static_cast<float>(
        (PUMP_DELAY_MS - (current_time_ms - last_change_time_ms)) / MS_TO_S);
    break;

  case POWER_RUNNING:
    adjusted_time =
        static_cast<float>((current_pump_data.time_range.running -
                            (current_time_ms - last_change_time_ms)) /
                           MS_TO_S);
    break;

  case POWER_RESTING:
    adjusted_time =
        static_cast<float>((current_pump_data.time_range.resting -
                            (current_time_ms - last_change_time_ms)) /
                           MS_TO_S);
    break;

  default:
    adjusted_time = 0.0f;
    break;
  }

  if (adjusted_time < 0) {
    adjusted_time = 0;
  }

  power.value = adjusted_time;
  if (client == nullptr) {
    send_num_message(power, POWER_TYPE_ID);
  } else {
    send_num_message_to_a_client(power, POWER_TYPE_ID, client);
  }
}

inline void power_off() {
  LOG_LN("Pump is OFF");

  current_pump_data.is_running = false;
  unsigned long current_time_ms = getCurrentTimeMs();
  unsigned long run_rest_time =
      current_time_ms - last_change_time_ms + PUMP_DELAY_MS;

  if (current_pump_data.mode == pump_MachineMode_POWER_OFF) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
  } else if (run_rest_time >= current_pump_data.time_range.running) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_RESTING),
        static_cast<float>(current_pump_data.time_range.resting / MS_TO_S));
  } else if (current_pump_data.mode == pump_MachineMode_AUTO &&
             !automate_mode_signal) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
  }
  last_change_time_ms = current_time_ms;

  digitalWrite(PUMP_RELAY_PIN, LOW);
}

void powerControl(void *pvParameters) {
  (void)pvParameters;
  uint32_t notificationValue;

  for (;;) {
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &notificationValue, portMAX_DELAY) ==
        pdTRUE) {

      if (notificationValue == POWER_OFF) {
        power_off();
      } else if (notificationValue == POWER_ON) {
        LOG_LN("Pump is ON");

        current_pump_data.is_running = true;
        last_change_time_ms = getCurrentTimeMs();
        update_and_send_power_status(
            static_cast<uint32_t>(PowerStatus::POWER_RUNNING),
            static_cast<float>(current_pump_data.time_range.running / MS_TO_S));

        digitalWrite(PUMP_RELAY_PIN, HIGH);
      } else {
        LOG_LN("Invalid notification value");
      }
    }
  }
}