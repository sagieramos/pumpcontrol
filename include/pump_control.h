#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include "dev_or_prod.h"
#include "network.h"
#include <transcode_pump_control.h>

#define PUMP_DELAY pdMS_TO_TICKS(5000)

constexpr int FLOAT_SIGNAL_PIN = 21;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr size_t EEPROM_SIZE_CTL =
    sizeof(pump_TimeRange) + MAGIC_NUMBER_SIZE + sizeof(float);

extern TaskHandle_t runMachineTask;
extern float min_voltage;

void store_time_range();
void switch_pump(bool state);

void runMachine(void *parameter);

void send_all_power_status_and_type(uint32_t clientId);

pump_ControlData &get_current_control_data();

extern SemaphoreHandle_t controlDataMutex;
extern TimerHandle_t delayTimer;
extern bool pumpState;

/**
 * @brief Processes a message and performs an action based on the first byte of
 * the data.
 *
 * This function interprets the first byte of the given data as an index and
 * uses it to select and call an appropriate message handler from a predefined
 * array of function pointers. Each handler processes the rest of the data
 * accordingly.
 *
 * @param data A pointer to the array of bytes containing the message data. The
 * first byte of this array is used to determine which handler to invoke.
 * @param len  The length of the data array. This includes the first byte used
 * for handler selection.
 *
 * @note The function ensures that the index (first byte of data) is within the
 * bounds of the function pointer array. If the index is out of bounds, a debug
 * message indicating an invalid action index is printed.
 */
void receive_msg_and_perform_action(uint8_t *data, size_t len,
                                    uint8_t msg_type);

#endif // PUMP_CONTROL_H