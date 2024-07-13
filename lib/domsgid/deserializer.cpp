#include "domsgid.h"
#include <arpa/inet.h>
// Helper function to deserialize a single DoId message
bool _deserialize_do_id(const uint8_t *buffer, size_t buffer_size, DoId &obj,
                        size_t &bytes_read) {
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_size);
  bool status = pb_decode(&stream, DoId_fields, &obj);

  if (status) {
    // Convert fields from network byte order to host byte order
    obj.id = ntohl(obj.id); // Convert int32_t to host byte order

    // Convert float from network byte order to host byte order
    uint32_t value_as_int;
    memcpy(&value_as_int, &obj.value, sizeof(value_as_int));
    value_as_int = ntohl(value_as_int);
    memcpy(&obj.value, &value_as_int, sizeof(value_as_int));

    bytes_read = buffer_size - stream.bytes_left;
    DEBUG_SERIAL_PRINTLN("Data deserialized successfully");
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to deserialize data");
    DEBUG_SERIAL_PRINTLN(PB_GET_ERROR(&stream));
  }

  return status;
}

// Function to deserialize a single DoId message
int deserialize_do_id(const uint8_t *buffer, size_t buffer_size, DoId &obj) {
  if (buffer == nullptr || buffer_size == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid input parameters");
    return -1;
  }

  size_t bytes_read = 0;
  if (_deserialize_do_id(buffer, buffer_size, obj, bytes_read)) {
    return bytes_read;
  }

  return -1;
}

int deserialize_do_id(const uint8_t *buffer, size_t buffer_size,
                      size_t &bytes_read_total, DoId *objs, size_t max_objs) {
  if (objs == nullptr || buffer == nullptr || buffer_size == 0 ||
      max_objs == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid input parameters");
    return -1;
  }

  bytes_read_total = 0;
  size_t num_objs = 0;

  while (bytes_read_total < buffer_size && num_objs < max_objs) {
    // Check if there are enough bytes for the length prefix
    if (buffer_size - bytes_read_total < sizeof(uint32_t)) {
      DEBUG_SERIAL_PRINTLN("Not enough bytes left for length prefix");
      break;
    }

    // Safely read message length (handle potential endianness issues)
    uint32_t message_len = 0;
    memcpy(&message_len, buffer + bytes_read_total, sizeof(uint32_t));
    bytes_read_total += sizeof(uint32_t);
    message_len = ntohl(message_len); // Convert to host byte order

    // Check for invalid message length
    if (message_len == 0 || message_len > buffer_size - bytes_read_total) {
      DEBUG_SERIAL_PRINTLN("Invalid message length");
      break;
    }

    size_t bytes_read = 0;
    bool status = _deserialize_do_id(buffer + bytes_read_total, message_len,
                                     objs[num_objs], bytes_read);

    if (!status) {
      DEBUG_SERIAL_PRINTF("Failed to deserialize message %zu\n", num_objs);
      break;
    }

    bytes_read_total += bytes_read;
    num_objs++;

    // Check for buffer overrun
    if (bytes_read_total > buffer_size) {
      DEBUG_SERIAL_PRINTLN("Buffer overrun");
      break;
    }
  }

  DEBUG_SERIAL_PRINTF("Total bytes read: %zu\n", bytes_read_total);
  DEBUG_SERIAL_PRINTF("Total messages read: %zu\n", num_objs);

  return num_objs;
}
