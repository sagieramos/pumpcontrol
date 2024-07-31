#include "pump_control.h"
#include "sensors.h"
#include "type_id.h"
#include <str_num_msg_transcode.h>

#define BUFFER_SIZE 16

// Timer callback to handle pump ON
void power_on_cb(TimerHandle_t xTimer) {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpState = true;
  DEBUG_SERIAL_PRINTLN("Pump is ON");
  Num msg = Num_init_default;
  msg.key = PowerKey::POWER_ON;

  {
    uint8_t buffer[BUFFER_SIZE];
    size_t buffer_size = BUFFER_SIZE;
    serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID, send_binary_data);

    if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
      pump_ControlData &ctrl = get_current_control_data();
      ctrl.is_running = true;
      xSemaphoreGive(controlDataMutex);
    } else {
      DEBUG_SERIAL_PRINTLN(
          "Failed to acquire controlDataMutex in pumpControlCallback()");
    }
  }

  msg.key = ConfigKey::CONFIG_RUNNING_STATE;
  msg.value = 1.0f;

  uint8_t buffer[BUFFER_SIZE];
  size_t buffer_size = BUFFER_SIZE;
  if (serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID,
                    send_binary_data)) {
    DEBUG_SERIAL_PRINTF("Sent running state message. key: %d, value: %f\n",
                        msg.key, msg.value);
    DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize running state message");
  }
}

void power_off() {
  digitalWrite(PUMP_RELAY_PIN, LOW);
  pumpState = false;
  DEBUG_SERIAL_PRINTLN("Pump is OFF");
  Num msg = Num_init_default;
  msg.key = PowerKey::POWER_OFF;

  {
    uint8_t buffer[BUFFER_SIZE];
    size_t buffer_size = BUFFER_SIZE;
    serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID, send_binary_data);

    if (xSemaphoreTake(controlDataMutex, portMAX_DELAY) == pdTRUE) {
      pump_ControlData &ctrl = get_current_control_data();
      ctrl.is_running = false;
      xSemaphoreGive(controlDataMutex);
    } else {
      DEBUG_SERIAL_PRINTLN(
          "Failed to acquire controlDataMutex in pumpControlCallback()");
    }
  }

  msg.key = ConfigKey::CONFIG_RUNNING_STATE;
  msg.value = 0.0f;

  uint8_t buffer[BUFFER_SIZE];
  size_t buffer_size = BUFFER_SIZE;
  if (serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID,
                    send_binary_data)) {
    DEBUG_SERIAL_PRINTF("Sent running state message. key: %d, value: %f\n",
                        msg.key, msg.value);
    DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize running state message");
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
  DEBUG_SERIAL_PRINTF("Pump will be ON in %d seconds\n", PUMP_DELAY / 1000);

  Num msg = Num_init_default;
  msg.key = PowerKey::POWER_READY;

  uint8_t buffer[BUFFER_SIZE];
  size_t buffer_size = BUFFER_SIZE;
  if (serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID,
                    send_binary_data)) {
    DEBUG_SERIAL_PRINTF("Sent power ready message. key: %d, value: %f\n",
                        msg.key, msg.value);
    DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize power ready message");
  }
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

    Num msg = Num_init_default;
    msg.key = PowerKey::POWER_READY;

    uint8_t buffer[BUFFER_SIZE];
    size_t buffer_size = BUFFER_SIZE;
    if (serialize_num(msg, buffer, &buffer_size, POWER_TYPE_ID,
                      send_binary_data)) {
      DEBUG_SERIAL_PRINTF("Sent power ready message. key: %d, value: %f\n",
                          msg.key, msg.value);
      DEBUG_SERIAL_PRINTF("Buffer size: %d\n", buffer_size);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to serialize power ready message");
    }
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
