#ifndef TRANS_PUMP_CONTROL_H
#define TRANS_PUMP_CONTROL_H

#include "./protoc/pump_control_data.pb.h"

/**
 * @brief Initializes a `pump_TimeRange` message with running and resting times.
 *
 * This function sets up a `pump_TimeRange` structure with the provided running
 * and resting times. The message is initialized to zero before setting these
 * values.
 *
 * @param time_range A reference to the `pump_TimeRange` structure to be
 * initialized.
 * @param running    The running time to be assigned to the `pump_TimeRange`
 * message.
 * @param resting    The resting time to be assigned to the `pump_TimeRange`
 * message.
 *
 * @note The `time_range` structure is first zeroed out using
 * `pump_TimeRange_init_zero` before setting the `running` and `resting` values.
 */
void create_pump_time_range(pump_TimeRange &time_range, uint32_t running,
                            uint32_t resting);

/**
 * @brief Initializes a `pump_ControlData` message with a mode, running state,
 * and optional time range.
 *
 * This function sets up a `pump_ControlData` structure with the provided mode,
 * running state, and an optional time range. The message is initialized to zero
 * before setting these values.
 *
 * @param control_data A reference to the `pump_ControlData` structure to be
 * initialized.
 * @param mode          The machine mode to be assigned to the
 * `pump_ControlData` message.
 * @param is_running    A boolean indicating whether the pump is running.
 * @param time_range    A pointer to the `pump_TimeRange` structure to be
 * included in the `pump_ControlData` message.
 *
 * @note If `time_range` is not nullptr, it sets `has_time_range` to true and
 * assigns the `time_range` to the message.
 */
void create_control_data(pump_ControlData &control_data, pump_MachineMode mode,
                         bool is_running, pump_TimeRange *time_range);

/**
 * @brief Serializes a `pump_TimeRange` message into a buffer with a type ID.
 *
 * This function encodes a `pump_TimeRange` message into a byte buffer,
 * prepending the buffer with a type ID. The encoding uses Protocol Buffers
 * (protobuf) to serialize the message fields. The buffer size is updated to
 * reflect the total size of the serialized message including the type ID.
 *
 * @param buffer       A pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size  A pointer to the size of the buffer. On entry, it should
 * contain the maximum size of the buffer. On exit, it will contain the size of
 * the serialized data including the type ID.
 * @param time_range   A pointer to the `pump_TimeRange` structure to be
 * serialized.
 * @param type_id      A type identifier that will be written as the first byte
 * of the buffer.
 *
 * @return `true` if serialization is successful, `false` otherwise.
 */
bool serialize_time_range(uint8_t *buffer, size_t *buffer_size,
                          const pump_TimeRange *time_range, uint8_t type_id);

/**
 * @brief Deserializes a `pump_TimeRange` message from a buffer.
 *
 * This function decodes a `pump_TimeRange` message from a byte buffer. It
 * assumes that the buffer starts with a type ID followed by the serialized
 * `pump_TimeRange` message data. The type ID is ignored, and only the message
 * data is used for deserialization.
 *
 * @param buffer       A pointer to the buffer containing the serialized message
 * data with a type ID prefix.
 * @param buffer_size  The size of the buffer. This should be the total size of
 * the buffer including the type ID.
 * @param time_range   A reference to the `pump_TimeRange` structure where the
 * deserialized data will be stored.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */
bool deserialize_time_range(const uint8_t *buffer, size_t buffer_size,
                            pump_TimeRange &time_range);

/**
 * @brief Serializes a `pump_ControlData` message into a buffer with a type ID.
 *
 * This function encodes a `pump_ControlData` message into a byte buffer,
 * prepending the buffer with a type ID. The encoding uses Protocol Buffers
 * (protobuf) to serialize the message fields. The buffer size is updated to
 * reflect the total size of the serialized message including the type ID.
 *
 * @param buffer       A pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size  A pointer to the size of the buffer. On entry, it should
 * contain the maximum size of the buffer. On exit, it will contain the size of
 * the serialized data including the type ID.
 * @param control_data A pointer to the `pump_ControlData` structure to be
 * serialized.
 * @param type_id      A type identifier that will be written as the first byte
 * of the buffer.
 *
 * @return `true` if serialization is successful, `false` otherwise.
 */
bool serialize_control_data(uint8_t *buffer, size_t *buffer_size,
                            const pump_ControlData *control_data,
                            uint8_t type_id);

/**
 * @brief Deserializes a `pump_ControlData` message from a buffer.
 *
 * This function decodes a `pump_ControlData` message from a byte buffer. It
 * assumes that the buffer starts with a type ID followed by the serialized
 * `pump_ControlData` message data. The type ID is ignored, and only the message
 * data is used for deserialization.
 *
 * @param buffer       A pointer to the buffer containing the serialized message
 * data with a type ID prefix.
 * @param buffer_size  The size of the buffer. This should be the total size of
 * the buffer including the type ID.
 * @param control_data A pointer to the `pump_ControlData` structure where the
 * deserialized data will be stored.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */
bool deserialize_control_data(const uint8_t *buffer, size_t buffer_size,
                              pump_ControlData *control_data);

#endif // TRANS_PUMP_CONTROL_H