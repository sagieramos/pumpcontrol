#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

constexpr unsigned long MS_TO_S = 1000;
constexpr unsigned long PUMP_DELAY_MS = 6000;
constexpr unsigned long DELAY_S = PUMP_DELAY_MS / MS_TO_S;

Num power = Num_init_default;

unsigned long last_change_time_s = 0;

// Helper function to update and send power status
void update_and_send_power_status(uint32_t key, float value) {
  power.key = key;
  power.value = value;
  send_num_message(power, POWER_TYPE_ID);
}

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpState = true;
  DEBUG_SERIAL_PRINTLN("Pump is ON");

  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &ctrl = get_current_control_data();
    ctrl.is_running = true;
    last_change_time_s = getCurrentTimeMs() / MS_TO_S;
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_RUNNING),
        static_cast<float>(ctrl.time_range.running));
    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to acquire controlDataMutex in power_on_cb()");
  }
}

void power_off() {
  digitalWrite(PUMP_RELAY_PIN, LOW);
  pumpState = false;
  DEBUG_SERIAL_PRINTLN("Pump is OFF");

  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &ctrl = get_current_control_data();
    ctrl.is_running = false;
    unsigned long current_time_s = getCurrentTimeMs() / MS_TO_S;

    if (current_time_s - last_change_time_s <= ctrl.time_range.resting ||
        ctrl.mode == pump_MachineMode_POWER_OFF) {
      update_and_send_power_status(
          static_cast<uint32_t>(PowerStatus::POWER_INACTIVE), 0.0f);
    } else {
      update_and_send_power_status(
          static_cast<uint32_t>(PowerStatus::POWER_RESTING),
          static_cast<float>(ctrl.time_range.resting));
    }
    last_change_time_s = current_time_s;
    xSemaphoreGive(controlDataMutex);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to acquire controlDataMutex in power_off()");
  }
}

// Start a timer to turn on the pump
void startPumpDelayTimer() {
  if (delayTimer == nullptr) {
    delayTimer = xTimerCreate("PumpDelayTimer", pdMS_TO_TICKS(PUMP_DELAY_MS),
                              pdFALSE, 0, power_on_cb);
    if (delayTimer == nullptr) {
      DEBUG_SERIAL_PRINTLN("Failed to create PumpDelayTimer");
      return;
    }
  }

  if (xTimerStart(delayTimer, 0) != pdPASS) {
    DEBUG_SERIAL_PRINTLN("Failed to start PumpDelayTimer");
  } else {
    DEBUG_SERIAL_PRINTLN("Pump delay timer started");
    DEBUG_SERIAL_PRINTF("Pump will be ON in %lu seconds\n", DELAY_S);

    last_change_time_s = getCurrentTimeMs() / MS_TO_S;
    update_and_send_power_status(
        static_cast<uint32_t>(PowerStatus::POWER_READY),
        static_cast<float>(DELAY_S));
  }
}

// Stop and clean up the timer
void stopAndCleanupTimer() {
  if (delayTimer != nullptr) {
    if (xTimerStop(delayTimer, 0) != pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to stop PumpDelayTimer");
    }
    if (xTimerDelete(delayTimer, 0) != pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to delete PumpDelayTimer");
    }
    delayTimer = nullptr;
    DEBUG_SERIAL_PRINTLN("Pump delay timer stopped");
  }
}

// Switch pump state with proper handling
void switch_pump(bool state) {
  if (state == pumpState)
    return;

  DEBUG_SERIAL_PRINTF("Switching pump state to %s\n", state ? "ON" : "OFF");

  pumpState = state;

  if (pumpState) {
    startPumpDelayTimer();
  } else {
    stopAndCleanupTimer();
    power_off();
  }
}

void send_serialized_message(Num &value, uint8_t type_id,
                             AsyncWebSocketClient *client) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;

  if (serialize_num(value, buffer, &buffer_size, type_id)) {
    client->binary(buffer, buffer_size);
    DEBUG_SERIAL_PRINTF("Sent message. key: %d, value: %f\n", value.key,
                        value.value);
    DEBUG_SERIAL_PRINTF("Type ID: %d\n", type_id);
    DEBUG_SERIAL_PRINTF("Buffer size: %zu\n", buffer_size);
    DEBUG_SERIAL_PRINTF("Client ID: %d\n", client->id());
    DEBUG_SERIAL_PRINTLN("====================================");
  } else {
    DEBUG_SERIAL_PRINTF("Failed to serialize power message\n");
  }
}

void send_all_power_status_and_type(AsyncWebSocketClient *client) {
  pump_ControlData &ctrl = get_current_control_data();
  PowerStatus power_status = static_cast<PowerStatus>(power.key);
  unsigned long current_time_s = getCurrentTimeMs() / MS_TO_S;

  switch (power_status) {
  case POWER_READY:
    power.value =
        static_cast<float>(current_time_s - last_change_time_s - DELAY_S);
    break;
  case POWER_RUNNING:
    power.value = static_cast<float>(ctrl.time_range.running -
                                     (current_time_s - last_change_time_s));
    break;
  case POWER_RESTING:
    power.value = static_cast<float>(ctrl.time_range.resting -
                                     (current_time_s - last_change_time_s));
    break;
  default:
    power.value = 0.0f;
    break;
  }

  send_serialized_message(power, POWER_TYPE_ID, client);
}
