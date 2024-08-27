#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

constexpr unsigned long MS_TO_S = 1000;
constexpr unsigned long PUMP_DELAY_MS = 6000;

Num power = Num_init_default;

unsigned long last_change_time_ms = 0;

// Helper function to update and send power status
void update_and_send_power_status(uint32_t key, float value) {
  power.key = key;
  power.value = value;

  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(power, buffer, &buffer_size, POWER_TYPE_ID,
                    send_binary_data)) {
    LOG_F("Sent Num message. key: %d, value: %f | Type ID: %d | "
          "Buffer size: %d\n",
          power.key, power.value, POWER_TYPE_ID, buffer_size);
  } else {
    LOG_F("Failed to serialize power message\n");
  }
}

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  pumpState = true;
  LOG_LN("Pump is ON");

  current_pump_data.is_running = true;
  last_change_time_ms = getCurrentTimeMs();
  update_and_send_power_status(
      static_cast<uint32_t>(PowerStatus::POWER_RUNNING),
      static_cast<float>(
          (current_pump_data.time_range.running - PUMP_DELAY_MS) / MS_TO_S));

  digitalWrite(PUMP_RELAY_PIN, HIGH);
}

void power_off() {
  pumpState = false;
  LOG_LN("Pump is OFF");

  current_pump_data.is_running = false;
  unsigned long current_time_ms = getCurrentTimeMs();
  unsigned long run_rest_time =
      current_time_ms - last_change_time_ms + PUMP_DELAY_MS;

  if (current_pump_data.mode == pump_MachineMode_POWER_OFF) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
  } else if (readingVolt < min_voltage) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_VOLTAGE_LOW), 0.0f);
  } else if (run_rest_time >= current_pump_data.time_range.running) {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_RESTING),
        static_cast<float>(current_pump_data.time_range.resting / MS_TO_S));
  } else {
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
  }
  last_change_time_ms = current_time_ms;

  digitalWrite(PUMP_RELAY_PIN, LOW);
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
    power_off();
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
    adjusted_time = static_cast<float>(
        (current_pump_data.time_range.running -
         (current_time_ms - last_change_time_ms) - PUMP_DELAY_MS) /
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
  send_num_message_to_a_client(power, POWER_TYPE_ID, client);
}
