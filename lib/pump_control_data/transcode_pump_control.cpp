#include "./transcode_pump_control.h"
#include <pb_decode.h>
#include <pb_encode.h>

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
