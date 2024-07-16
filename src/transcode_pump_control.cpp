#include "msg_type_identifier.h"
#include "network.h"
#include "pump_control.h"
#include <pump_control_data.pb.h>

bool serialize_control_data(uint8_t *buffer, size_t buffer_size,
                            size_t &encoded_size,
                            const pump_ControlData *control_data,
                            uint8_t type_identifier) {
  pb_ostream_t size_stream = PB_OSTREAM_SIZING;
  if (!pb_encode(&size_stream, pump_ControlData_fields, control_data)) {
    DEBUG_SERIAL_PRINTLN("Failed to calculate the size of the message");
    return false;
  }

  size_t message_size = size_stream.bytes_written;

  if (buffer_size < 1 + message_size) {
    DEBUG_SERIAL_PRINTF("Buffer size is too small: %d < %d\n", buffer_size,
                        1 + message_size);
    return false;
  }

  buffer[0] = type_identifier;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, buffer_size - 1);
  if (!pb_encode(&ostream, pump_ControlData_fields, control_data)) {
    return false;
  }

  encoded_size = ostream.bytes_written + 1;
  DEBUG_SERIAL_PRINTF("Encoded size: %d\n", encoded_size);

  return true;
}

bool deserialize_control_data(const uint8_t *buffer, size_t buffer_size,
                              pump_ControlData *control_data,
                              uint8_t type_identifier) {
  // Check if the type identifier matches
  if (buffer_size == 0 || buffer[0] != type_identifier) {
    DEBUG_SERIAL_PRINTLN("Type identifier mismatch or buffer is empty");
    DEBUG_SERIAL_PRINTF("Buffer type identifier: %d\n", buffer[0]);
    return false;
  }

  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&istream, pump_ControlData_fields, control_data);
}

void send_control_data(const size_t client_id) {
  if (client_id == 0) {
    return;
  }
  unsigned char buffer[1024];

  pump_ControlData control_data = get_current_control_data();
  DEBUG_SERIAL_PRINTF("Control Data - Mode: %d\tRunning: %d\tTime Range - "
                      "Running: %d\tResting: %d\n",
                      control_data.mode, control_data.is_running,
                      control_data.time_range.running,
                      control_data.time_range.resting);
  size_t encoded_size;
  bool status =
      serialize_control_data(buffer, sizeof(buffer), encoded_size,
                             &control_data, TYPE_IDENTIFIER_PUMP_CONTROL_DATA);
#ifndef PRODUCTION
  if (status) {
    for (size_t i = 0; i < encoded_size; i++) {
      DEBUG_SERIAL_PRINTF("%02X ", buffer[i]);
    }

    // Deserialize the data
    pump_ControlData control_data_deserialized;
    if (deserialize_control_data(buffer, encoded_size,
                                 &control_data_deserialized,
                                 TYPE_IDENTIFIER_PUMP_CONTROL_DATA)) {
      DEBUG_SERIAL_PRINTLN("Deserialized data..............................:");
      DEBUG_SERIAL_PRINTF("Mode: %d\n", control_data_deserialized.mode);
      DEBUG_SERIAL_PRINTF("Running: %d\n",
                          control_data_deserialized.is_running);
      DEBUG_SERIAL_PRINTF("Time Range - Running: %d\n",
                          control_data_deserialized.time_range.running);
      DEBUG_SERIAL_PRINTF("Time Range - Resting: %d\n",
                          control_data_deserialized.time_range.resting);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize control data");
    }
    DEBUG_SERIAL_PRINTLN();
  }
#endif

  if (status && client_id == 0) {
    ws.binaryAll(buffer, encoded_size);
  } else if (status) {
    ws.binary(client_id, buffer, encoded_size);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize control data");
  }
}