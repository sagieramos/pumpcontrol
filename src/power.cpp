#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

#define PUMP_DELAY pdMS_TO_TICKS(6000)
constexpr unsigned long MS_TO_S = 1000;
constexpr unsigned long DELAY_S = PUMP_DELAY / MS_TO_S;

Num power = Num_init_default;

unsigned long last_change_time = 0;

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpState = true;
  DEBUG_SERIAL_PRINTLN("Pump is ON");
  power.key = static_cast<uint32_t>(PowerStatus::POWER_RUNNING);

  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &ctrl = get_current_control_data();
    ctrl.is_running = true;
    power.value = static_cast<float>(ctrl.time_range.running);
    xSemaphoreGive(controlDataMutex);

    last_change_time = getCurrentTimeMs() / MS_TO_S;
    send_num_message(power, POWER_TYPE_ID);

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
    xSemaphoreGive(controlDataMutex);
    unsigned long current_time = getCurrentTimeMs() / MS_TO_S;

    if (current_time - last_change_time < ctrl.time_range.resting) {
      power.key = static_cast<uint32_t>(PowerStatus::POWER_INACTIVE);
      power.value = 0.0f;
    } else {
      power.key = static_cast<uint32_t>(PowerStatus::POWER_RESTING);
      power.value = static_cast<float>(ctrl.time_range.resting);
    }
    last_change_time = current_time;
    send_num_message(power, POWER_TYPE_ID);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to acquire controlDataMutex in power_off()");
  }
}

// Start a timer to turn on the pump
void startPumpDelayTimer() {
  if (delayTimer == NULL) {
    // Create the timer
    delayTimer =
        xTimerCreate("PumpDelayTimer", PUMP_DELAY, pdFALSE, 0, power_on_cb);
    if (delayTimer == NULL) {
      DEBUG_SERIAL_PRINTLN("Failed to create PumpDelayTimer");
      return;
    }
  }
  // Start the timer
  if (xTimerStart(delayTimer, 0) != pdPASS) {
    DEBUG_SERIAL_PRINTLN("Failed to start PumpDelayTimer");
    return;
  }

  DEBUG_SERIAL_PRINTLN("Pump delay timer started");
  DEBUG_SERIAL_PRINTF("Pump will be ON in %d seconds\n", DELAY_S);

  last_change_time = getCurrentTimeMs() / MS_TO_S;

  power.key = static_cast<uint32_t>(PowerStatus::POWER_READY);
  power.value = static_cast<float>(DELAY_S);

  send_num_message(power, POWER_TYPE_ID);
}

// Stop and clean up the timer
void stopAndCleanupTimer() {
  if (delayTimer != NULL) {
    // Stop the timer
    if (xTimerStop(delayTimer, 0) != pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to stop PumpDelayTimer");
    }
    // Delete the timer
    if (xTimerDelete(delayTimer, 0) != pdPASS) {
      DEBUG_SERIAL_PRINTLN("Failed to delete PumpDelayTimer");
    }
    delayTimer = NULL;

    DEBUG_SERIAL_PRINTLN("Pump delay timer stopped");
  }
}

// Switch pump state with proper handling
void switch_pump(bool state) {
  if (state == pumpState) {
    return;
  }

  if (state) {
    startPumpDelayTimer();

  } else {
    stopAndCleanupTimer();
    power_off();
  }

  pumpState = state;
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
    DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
    DEBUG_SERIAL_PRINTF("Client ID: %d\n", client->id());
    DEBUG_SERIAL_PRINTLN("====================================");
  } else {
    DEBUG_SERIAL_PRINTF("Failed to serialize power message\n");
  }
}

void send_all_power_status_and_type(AsyncWebSocketClient *client) {

  pump_ControlData &ctrl = get_current_control_data();

  PowerStatus power_status = static_cast<PowerStatus>(power.key);
  const unsigned long current_time = getCurrentTimeMs() / MS_TO_S;

  switch (power_status) {
  case POWER_READY: {
    power.value = static_cast<float>(current_time - last_change_time - DELAY_S);
    break;
  }
  case POWER_RUNNING: {
    unsigned long current_run_time = current_time - last_change_time;
    power.value =
        static_cast<float>(ctrl.time_range.running - current_run_time);
    break;
  }
  case POWER_RESTING: {
    unsigned long current_rest_time = current_time - last_change_time;
    power.value =
        static_cast<float>(ctrl.time_range.resting - current_rest_time);
    break;
  }
  default:
    power.value = 0.0f;
    break;
  }

  send_serialized_message(power, POWER_TYPE_ID, client);
}
