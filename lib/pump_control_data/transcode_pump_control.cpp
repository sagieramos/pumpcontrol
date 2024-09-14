#include "./transcode_pump_control.h"
/* #include "dev_or_prod.h" */
#include <pb_decode.h>
#include <pb_encode.h>

/**
 * @brief Initializes a pump time range structure.
 *
 * @param time_range Reference to the pump_TimeRange structure to initialize.
 * @param running Duration for which the pump is running.
 * @param resting Duration for which the pump is resting.
 */
void create_pump_time_range(pump_TimeRange &time_range, uint32_t running,
                            uint32_t resting) {
  time_range = pump_TimeRange_init_zero;
  time_range.running = running;
  time_range.resting = resting;
}

/**
 * @brief Initializes a control data structure.
 *
 * @param control_data Reference to the pump_ControlData structure to
 * initialize.
 * @param mode Machine mode for the control data.
 * @param is_running Indicates whether the pump is running.
 * @param time_range Pointer to the time range data, or nullptr if not used.
 */
void create_control_data(pump_ControlData &control_data, pump_MachineMode mode,
                         bool is_running, pump_TimeRange *time_range) {
  control_data = pump_ControlData_init_zero;
  control_data.mode = mode;
  control_data.is_running = is_running;

  if (time_range != nullptr) {
    control_data.has_time_range = true;
    control_data.time_range = *time_range;
  }
}

/**
 * @brief Serializes a time range into a buffer.
 *
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to the size of the buffer; updated with the final
 * size.
 * @param time_range Pointer to the time range data to serialize.
 * @param type_id Type identifier for the message.
 * @param cb Optional callback function to call with the serialized data.
 * @return true if serialization was successful, false otherwise.
 */
bool serialize_time_range(const pump_TimeRange &time_range, uint8_t *buffer,
                          size_t *buffer_size, uint8_t type_id,
                          pump_codec_callback_t cb) {
  buffer[0] = type_id;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&ostream, pump_TimeRange_fields, &time_range);

  *buffer_size = ostream.bytes_written + 1;

  if (cb && status) {
    cb(static_cast<void *>(buffer), *buffer_size);
  }

  return status;
}

/**
 * @brief Deserializes a time range from a buffer.
 *
 * @param time_range Reference to the time range structure to populate.
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer.
 * @param cb Optional callback function to call with the deserialized data.
 * @return true if deserialization was successful, false otherwise.
 */
bool deserialize_time_range(pump_TimeRange &time_range, const uint8_t *buffer,
                            size_t buffer_size, pump_codec_callback_t cb) {
  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  bool status = pb_decode(&istream, pump_TimeRange_fields, &time_range);

  if (cb && status) {
    cb((void *)buffer, buffer_size);
  }

  /*   if (!status) {
      LOG_F("Decoding failed: %s\n", PB_GET_ERROR(&istream));
    } */

  return status;
}

/**
 * @brief Serializes control data into a buffer.
 *
 * @param control_data The control data to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to the size of the buffer; updated with the final
 * size.
 * @param type_id Type identifier for the message.
 * @param cb Optional callback function to call with the serialized data.
 * @return true if serialization was successful, false otherwise.
 */
bool serialize_control_data(const pump_ControlData &control_data,
                            uint8_t *buffer, size_t *buffer_size,
                            uint8_t type_id, pump_codec_callback_t cb) {
  buffer[0] = type_id;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&ostream, pump_ControlData_fields, &control_data);

  *buffer_size = ostream.bytes_written + 1;

  if (cb && status) {
    cb(static_cast<void *>(buffer), *buffer_size);
  }

  return status;
}

/**
 * @brief Deserializes control data from a buffer.
 *
 * @param control_data Reference to the control data structure to populate.
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer.
 * @param cb Optional callback function to call with the deserialized data.
 * @return true if deserialization was successful, false otherwise.
 */
bool deserialize_control_data(pump_ControlData &control_data,
                              const uint8_t *buffer, size_t buffer_size,
                              pump_codec_callback_t cb) {
  if (buffer_size == 0) {
    return false;
  }
  control_data = pump_ControlData_init_zero;

  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  bool status = pb_decode(&istream, pump_ControlData_fields, &control_data);

  if (cb && status) {
    cb(static_cast<void *>(&control_data), sizeof(control_data));
  }

  return status;
}
