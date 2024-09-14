#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include "dev_or_prod.h"
#include "network.h"
#include "type_id.h"
#include <EEPROM.h>
#include <PZEM004Tv30.h>

constexpr int FLOAT_SIGNAL_PIN = 21; // Float signal pin
constexpr int PUMP_RELAY_PIN = 12;   // Relay pin for pump

constexpr uint8_t MAGIC_NUMBER = 123; // Magic number for EEPROM data validity

// Sizes of individual data types
constexpr size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBER);
constexpr size_t PUMP_MODE_SIZE = sizeof(uint8_t);
constexpr size_t PUMP_TIME_RANGE_SIZE = sizeof(pump_TimeRange);
constexpr size_t MIN_VOLTAGE_SIZE = sizeof(float);
constexpr size_t TOTAL_ENERGY_CONSUMED_SIZE = sizeof(float);
constexpr size_t DAYS_IN_OPERATION_SIZE = sizeof(float);
constexpr size_t CRC_SIZE = sizeof(uint32_t);

// Calculate EEPROM offsets based on data sizes
constexpr int PUMP_MODE_ADDRESS = MAGIC_NUMBER_SIZE;
constexpr int PUMP_TIME_RANGE_ADDRESS = PUMP_MODE_ADDRESS + PUMP_MODE_SIZE;
constexpr int MIN_VOLTAGE_ADDRESS =
    PUMP_TIME_RANGE_ADDRESS + PUMP_TIME_RANGE_SIZE;
constexpr int TOTAL_ENERGY_CONSUMED_ADDRESS =
    MIN_VOLTAGE_ADDRESS + MIN_VOLTAGE_SIZE;
constexpr int DAYS_IN_OPERATION_ADDRESS =
    TOTAL_ENERGY_CONSUMED_ADDRESS + TOTAL_ENERGY_CONSUMED_SIZE;

// CRC is stored after all the data
constexpr int CRC_ADDRESS = DAYS_IN_OPERATION_ADDRESS + DAYS_IN_OPERATION_SIZE;

// Total EEPROM size required
constexpr size_t EEPROM_SIZE_CTL = CRC_ADDRESS + CRC_SIZE;

extern TaskHandle_t runMachineTaskHandle;
extern TaskHandle_t checkSignalHandle;
extern TaskHandle_t powerControlTask;
extern TaskHandle_t readPzemTaskHandle;
extern float min_voltage;
extern pump_ControlData current_pump_data;
extern float readingVolt;
extern Num power;
extern bool pumpState;
extern bool automate_mode_signal;
extern PZEM004Tv30 pzem;

void store_time_range(bool check_changed = true);
void store_pump_mode(bool check_changed = true);
bool is_valid_time_range(const pump_TimeRange &time_range);
void update_and_send_power_status(uint32_t key, float value);
void switch_pump(bool state);
void reset_energy();

void runMachineTask(void *parameter);
void checkSignalTask(void *parameter);
void powerControl(void *pvParameters);
void readPzemTask(void *parameter);

void send_all_power_status_and_type(AsyncWebSocketClient *client = nullptr);

extern TimerHandle_t delayTimer;

struct PzemData {
  float total_energy_consumed_in_operation; // in kWh
  unsigned long days_in_operation;          // in days
  unsigned long last_run_duration;          // in minutes
  unsigned long last_run_energy;            // in Wh
};

#define PzemData_size sizeof(PzemData)

void update_pzem_data(unsigned long durationMS);
void init_pzem_data();
bool enqueueMessage(const MessageData &msg_data);

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