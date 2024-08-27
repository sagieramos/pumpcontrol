#ifndef TYPE_ID_H
#define TYPE_ID_H

#include <stddef.h>
#include <stdint.h>

#define MAX_DATA_LENGTH 1024

/**
 * @typedef MsgHandler
 * @brief Type alias for a function pointer that handles messages.
 *
 * This typedef defines a type alias for a function pointer that takes a pointer
 * to data and its length as parameters and returns void. It is used to
 * reference different message handling functions.
 */
typedef void (*MsgHandler)(uint8_t *, size_t);

/**
 * @var receive_ptr
 * @brief Array of function pointers for handling different message types.
 *
 * This array maps type IDs to their respective handler functions. Each entry in
 * the array corresponds to a specific type of message, where:
 * - `receive_ptr[0]` is a placeholder function (`void_action`).
 * - `receive_ptr[1]` handles single configuration updates
 * (`receive_single_config`).
 * - `receive_ptr[2]` handles string messages (`receive_str`).
 * - `receive_ptr[3]` handles string and number messages (`receive_strnum`).
 * - `receive_ptr[4]` handles control data (`receive_control_data`).
 * - `receive_ptr[5]` handles pump time range data (`receive_pump_time_range`).
 */
extern MsgHandler receive_ptr[];

/**
 * @def VOID_TYPE_ID
 * @brief Type ID for placeholder or no-operation actions.
 *
 * This constant represents the type ID used for functions that do not perform
 * any meaningful action. It is mapped to the placeholder function in the
 * `receive_ptr` array. This type ID is used as a placeholder for unused or
 * invalid message types. It is also used to indicate no-operation actions.
 */
constexpr uint8_t VOID_TYPE_ID = 0x00;

/**
 * @def SINGLE_CONFIG_TYPE_ID
 * @brief Type ID for single configuration updates.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain single
 * configuration updates.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 *
 * This constant represents the type ID used for messages that contain single
 * configuration updates. It is mapped to the `receive_single_config` function
 * in the `receive_ptr` array.
 */
constexpr uint8_t SINGLE_CONFIG_TYPE_ID = 0x01;
constexpr uint8_t NUM_TYPE_ID = 0x01;

/**
 * @def STR_TYPE_ID
 * @brief Type identifier for string messages.
 *
 * This constant defines the unique type identifier for `str` (string) messages
 * in the message handling system. It is used to distinguish between different
 * types of messages during serialization and deserialization.
 *
 * Usage:
 * - This type identifier should be used when serializing `str` messages to
 *   ensure they are correctly identified during deserialization.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 *
 * @note The value `0x02` is assigned to string messages. Ensure that this value
 *       is unique and does not conflict with other type identifiers in the
 * system.
 */
constexpr uint8_t STR_TYPE_ID = 0x02;

/**
 * @def STR_NUM_TYPE_ID
 * @brief Type ID for string and number messages.
 *
 * This constant represents the type ID used for messages that contain both
 * strings and numbers. It is mapped to the `receive_strnum` function in the
 * `receive_ptr` array.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain both
 * strings and numbers.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 *
 * @note This type ID is used for messages that contain both strings and
 * numbers. It is different from the `STR_TYPE_ID` used for messages that
 * contain only strings.
 */
constexpr uint8_t STR_NUM_TYPE_ID = 0x03;

/**
 * @def CONTROL_DATA_TYPE_ID
 * @brief Type ID for control data messages.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain control
 * data.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 *
 * This constant represents the type ID used for messages that contain control
 * data. It is mapped to the `receive_control_data` function in the
 * `receive_ptr` array.
 */
constexpr uint8_t CONTROL_DATA_TYPE_ID = 0x04;

/**
 * @def PUMP_TIME_RANGE_TYPE_ID
 * @brief Type ID for pump time range messages.
 *
 * This constant represents the type ID used for messages that contain pump
 * time range data. It is mapped to the `receive_pump_time_range` function in
 * the `receive_ptr` array.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain pump
 * time range data.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 */
constexpr uint8_t PUMP_TIME_RANGE_TYPE_ID = 0x05;

/**
 * @def AUTH_TYPE_ID
 * @brief Type ID for authentication messages.
 *
 * This constant represents the type ID used for messages that contain
 * authentication data. It is used to identify messages that contain
 * authentication information during serialization and deserialization.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain
 * authentication data.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 */
constexpr uint8_t AUTH_TYPE_ID = 0x06;

/**
 * @enum ConfigKey
 * @brief Enum representing the configuration keys for pump control settings.
 *
 * This enum defines the possible keys used to identify different configuration
 * parameters within the pump control system. These keys are used to deserialize
 * incoming messages and update specific settings of the pump control data.
 *
 * @note The keys are assigned specific values to avoid conflicts with other
 *       message types or configuration parameters. Ensure that the keys used
 *       in messages match these defined values to avoid unexpected behavior.
 */

/**
 * @def VOLTAGE_TYPE_ID
 * @brief Type ID for voltage sensor messages.
 *
 * This constant represents the type ID used for messages that contain voltage
 * sensor data. It is used to identify messages that contain voltage sensor
 * information during serialization and deserialization.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain voltage
 * sensor data.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 */

constexpr uint8_t VOLTAGE_TYPE_ID = 0x07;

/**
 * @def POWER_TYPE_ID
 * @brief Type ID for power control messages.
 *
 * This constant represents the type ID used for messages that contain power
 * control data. It is used to identify messages that contain power control
 * information during serialization and deserialization.
 *
 * usage:
 * - This type ID should be used when serializing messages that contain power
 * control data.
 * - It helps in routing the messages to appropriate handlers based on their
 * type.
 */

enum VoltageKey { VOLTAGE, MIN_VOLTAGE, MAX_VOLTAGE };

constexpr uint8_t POWER_TYPE_ID = 0x08;

enum PowerStatus {
  POWER_INACTIVE = 0,
  POWER_READY,
  POWER_RUNNING,
  POWER_RESTING,
  POWER_VOLTAGE_LOW
};

enum ConfigKey {
  VOLTAGE_SENSOR = 1,

  /**
   * @brief Key for setting the pump mode.
   *
   * This key is used to update the operating mode of the pump. The value should
   * be of type `pump_MachineMode`, which represents different modes of
   * operation.
   */
  CONFIG_MODE = 101,

  /**
   * @brief Key for setting the running time of the pump.
   *
   * This key is used to update the duration for which the pump should remain
   * running. The value should be a `uint32_t` representing the time in
   * milliseconds.
   */
  CONFIG_RUNNING_TIME,

  /**
   * @brief Key for setting the resting time of the pump.
   *
   * This key is used to update the duration for which the pump should remain
   * in a resting state. The value should be a `uint32_t` representing the time
   * in milliseconds.
   */
  CONFIG_RESTING_TIME,

  /**
   * @brief Key for reporting the pump's running state.
   *
   * This key is used to report whether the pump is currently running or not.
   * The value should be of type `float`, where a value of `1.0` represents
   * that the pump is running, and `0.0` represents that it is not running.
   */
  CONFIG_RUNNING_STATE,

  /**
   * @brief Key for setting the minimum voltage threshold.
   *
   * This key is used to set the minimum voltage threshold for the pump. The
   * value should be of type `float` representing the minimum voltage level
   * required for the pump to operate.
   */
  CONFIG_MIN_VOLTAGE,

  /**
   * @brief Maximum value for configuration keys.
   *
   * This value represents the maximum key value used for configuration
   * parameters. It is used to validate incoming messages and ensure that the
   * keys are within the expected range.
   */
  CONFIG_KEY_MAX
};

#endif // TYPE_ID_H
