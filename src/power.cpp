#include "network.h"
#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"

enum Timer { INACTIVE = 0, RESTING, RUNNING };

Num power = Num_init_default;
Num power_status = Num_init_default;

unsigned int last_change_time = 0;

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpState = true;
  DEBUG_SERIAL_PRINTLN("Pump is ON");
  power.key = static_cast<uint32_t>(Timer::RUNNING);

  if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
    pump_ControlData &ctrl = get_current_control_data();
    ctrl.is_running = true;
    power.value = ctrl.time_range.running;
    xSemaphoreGive(controlDataMutex);

    last_change_time = getCurrentTimeMs();
    send_num_message(power, POWER_TYPE_ID);

    // Send power status
    power_status.key = static_cast<uint32_t>(PowerStatus::POWER_ON);
    power_status.value = 0;
    send_num_message(power_status, POWER_STATUS_ID);

  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in pumpControlCallback()");
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
    unsigned int current_time = getCurrentTimeMs();

    if (current_time - last_change_time < ctrl.time_range.resting) {
      power.key = static_cast<uint32_t>(Timer::INACTIVE);
      power.value = 0;
    } else {
      last_change_time = current_time;
      power.key = static_cast<uint32_t>(Timer::RESTING);
      power.value = ctrl.time_range.resting;
    }
    send_num_message(power, POWER_TYPE_ID);
  } else {
    DEBUG_SERIAL_PRINTLN(
        "Failed to acquire controlDataMutex in pumpControlCallback()");
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

  const unsigned int delay_s = PUMP_DELAY / 1000;

  DEBUG_SERIAL_PRINTLN("Pump delay timer started");
  DEBUG_SERIAL_PRINTF("Pump will be ON in %d seconds\n", delay_s);

  power_status.key = static_cast<uint32_t>(PowerStatus::POWER_READY);
  power_status.value = delay_s;
  send_num_message(power_status, POWER_STATUS_ID);
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

    power_status.key = PowerStatus::POWER_OFF;
    power_status.value = 0;
    send_num_message(power_status, POWER_STATUS_ID);
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

void send_serialized_message(Num &value, uint8_t type_id, uint32_t clientId) {
  uint8_t buffer[NUM_BUFFER_SIZE];
  size_t buffer_size = NUM_BUFFER_SIZE;
  if (serialize_num(value, buffer, &buffer_size, type_id)) {
    ws.binary(clientId, buffer, buffer_size);
    DEBUG_SERIAL_PRINTF("Sent message. key: %d, value: %f\n", value.key,
                        value.value);
    DEBUG_SERIAL_PRINTF("Type ID: %d\n", type_id);
    DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
    DEBUG_SERIAL_PRINTF("Client ID: %d\n", clientId);
    DEBUG_SERIAL_PRINTLN("====================================");
  } else {
    DEBUG_SERIAL_PRINTF("Failed to serialize power message\n");
  }
}

void send_all_power_status_and_type(uint32_t clientId) {
  send_serialized_message(power, POWER_TYPE_ID, clientId);
  send_serialized_message(power_status, POWER_STATUS_ID, clientId);
}
