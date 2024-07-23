#include "./transcode_pump_control.h"
#include <pb_decode.h>
#include <pb_encode.h>

void create_pump_time_range(pump_TimeRange &time_range, uint32_t running,
                            uint32_t resting) {
  time_range = pump_TimeRange_init_zero;
  time_range.running = running;
  time_range.resting = resting;
}

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

bool serialize_time_range(uint8_t *buffer, size_t *buffer_size,
                          const pump_TimeRange *time_range, uint8_t type_id) {
  pb_ostream_t size_stream = PB_OSTREAM_SIZING;
  if (!pb_encode(&size_stream, pump_TimeRange_fields, time_range)) {
    return false;
  }

  size_t message_size = size_stream.bytes_written;

  if (*buffer_size < 1 + message_size) {
    return false;
  }

  buffer[0] = type_id;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  if (!pb_encode(&ostream, pump_TimeRange_fields, time_range)) {
    return false;
  }

  *buffer_size = ostream.bytes_written + 1;

  return true;
}

bool deserialize_time_range(const uint8_t *buffer, size_t buffer_size,
                            pump_TimeRange &time_range) {
  if (buffer_size == 0) {
    return false;
  }
  time_range = pump_TimeRange_init_zero;

  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&istream, pump_TimeRange_fields, &time_range);
}

bool serialize_control_data(uint8_t *buffer, size_t *buffer_size,
                            const pump_ControlData *control_data,
                            uint8_t type_id) {
  pb_ostream_t size_stream = PB_OSTREAM_SIZING;
  if (!pb_encode(&size_stream, pump_ControlData_fields, control_data)) {
    return false;
  }

  size_t message_size = size_stream.bytes_written;

  if (*buffer_size < 1 + message_size) {
    return false;
  }

  buffer[0] = type_id;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  if (!pb_encode(&ostream, pump_ControlData_fields, control_data)) {
    return false;
  }

  *buffer_size = ostream.bytes_written + 1;

  return true;
}

bool deserialize_control_data(const uint8_t *buffer, size_t buffer_size,
                              pump_ControlData *control_data) {
  if (buffer_size == 0) {
    return false;
  }
  control_data = {};

  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&istream, pump_ControlData_fields, control_data);
}
