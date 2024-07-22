#ifndef TYPE_ID_H
#define TYPE_ID_H

#include <stddef.h>
#include <stdint.h>

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
 * - `receive_ptr[3]` handles string and number messages (`receive_Strnum`).
 * - `receive_ptr[4]` handles control data (`receive_control_data`).
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
 * strings and numbers. It is mapped to the `receive_Strnum` function in the
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
 *
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

#endif // TYPE_ID_H
