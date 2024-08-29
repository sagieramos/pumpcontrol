#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include "dev_or_prod.h"
#include "network.h"
#include <transcode_pump_control.h>

constexpr int FLOAT_SIGNAL_PIN = 21; // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;   // Relay pin for pump

constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity

// Sizes of individual data types
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr size_t PUMP_MODE_SIZE = sizeof(uint8_t);
constexpr size_t PUMP_TIME_RANGE_SIZE = sizeof(pump_TimeRange);
constexpr size_t MIN_VOLT_SIZE = sizeof(float);

// Calculate EEPROM offsets based on data sizes
constexpr int PUMP_MODE_ADDRESS = MAGIC_NUMBER_SIZE;
constexpr int PUMP_TIME_RANGE_ADDRESS = PUMP_MODE_ADDRESS + PUMP_MODE_SIZE;
constexpr int MIN_VOLTAGE_ADDRESS =
    PUMP_TIME_RANGE_ADDRESS + PUMP_TIME_RANGE_SIZE;

// Total EEPROM size required
constexpr size_t EEPROM_SIZE_CTL = MIN_VOLTAGE_ADDRESS + MIN_VOLT_SIZE;

extern TaskHandle_t runMachineTask;
extern TaskHandle_t checkSignalTask;
extern float min_voltage;
extern pump_ControlData current_pump_data;
extern unsigned long lastChangeTime;
extern float readingVolt;
extern Num power;
extern bool pumpState;
extern bool automate_mode_signal;

void store_time_range(bool check_changed = true);
void store_pump_mode(bool check_changed = true);
bool is_valid_time_range(const pump_TimeRange &time_range);
void update_and_send_power_status(uint32_t key, float value);
void switch_pump(bool state);

void runMachine(void *parameter);
void checkSignal(void *parameter);

void send_all_power_status_and_type(AsyncWebSocketClient *client = nullptr);

extern TimerHandle_t delayTimer;

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