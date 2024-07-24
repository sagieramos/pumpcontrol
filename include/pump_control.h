#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include "dev_or_prod.h"
#include "network.h"
#include <transcode_pump_control.h>

constexpr uint32_t TIME_ON(40 * 60000);
constexpr uint32_t TIME_OFF(20 * 60000);

constexpr int FLOAT_SIGNAL_PIN = 21;  // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;    // Relay pin for pump
constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr size_t EEPROM_SIZE_CTL = sizeof(pump_TimeRange) + MAGIC_NUMBER_SIZE;

extern TaskHandle_t runMachineTask;

void send_control_data(const size_t client_id = 0);
void store_pump_time_range(const pump_TimeRange *time_range);
void store_pump_time_range();
void runMachine(void *parameter);
pump_ControlData &get_current_control_data();

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
void receive_msg_and_perform_action(uint8_t *data, size_t len);

#endif // PUMP_CONTROL_H