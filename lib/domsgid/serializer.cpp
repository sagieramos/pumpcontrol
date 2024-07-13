#include "domsgid.h"
#include <arpa/inet.h>

bool _serialize_do_id(uint8_t *buffer, size_t buffer_size, int32_t id,
                      float value, size_t &bytes_written) {
  DoId message = DoId_init_zero;
  message.id = htonl(id); // Convert int32_t to network byte order

  // Convert float to network byte order
  uint32_t value_as_int;
  memcpy(&value_as_int, &value, sizeof(value));
  value_as_int = htonl(value_as_int);
  memcpy(&message.value, &value_as_int, sizeof(value_as_int));

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  bool status = pb_encode(&stream, DoId_fields, &message);

  if (status) {
    bytes_written = stream.bytes_written;
    DEBUG_SERIAL_PRINTLN("Data serialized successfully");
    DEBUG_SERIAL_PRINTF("Encoded size: %zu\n", stream.bytes_written);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to serialize data");
    DEBUG_SERIAL_PRINTLN(PB_GET_ERROR(&stream));
  }

  return status;
}

int serialize_do_id(uint8_t *buffer, size_t buffer_size, const DoId &obj) {
  if (buffer == nullptr || buffer_size == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid input parameters");
    return -1;
  }

  size_t bytes_written = 0;
  if (_serialize_do_id(buffer, buffer_size, obj.id, obj.value, bytes_written)) {
    return bytes_written;
  }

  DEBUG_SERIAL_PRINTLN("Serialization failed for a single DoId object");
  return -1;
}

int serialize_do_id(uint8_t *buffer, size_t buffer_size, const DoId *objs,
                    size_t num_objs) {
  if (objs == nullptr || buffer == nullptr || buffer_size == 0) {
    DEBUG_SERIAL_PRINTLN("Invalid input parameters");
    return -1;
  }

  int bytes_written_total = 0;
  bool overall_status = true;

  for (size_t i = 0; i < num_objs; ++i) {
    size_t bytes_written = 0;
    bool status = _serialize_do_id(buffer + bytes_written_total,
                                   buffer_size - bytes_written_total,
                                   objs[i].id, objs[i].value, bytes_written);

    if (!status) {
      DEBUG_SERIAL_PRINTF("Failed to serialize message %zu\n", i);
      overall_status = false;
      break;
    }

    bytes_written_total += bytes_written;

    // Check for buffer overrun
    if (bytes_written_total > buffer_size) {
      DEBUG_SERIAL_PRINTLN("Buffer overrun detected");
      overall_status = false;
      break;
    }
  }

  DEBUG_SERIAL_PRINTF("Total bytes written: %d\n", bytes_written_total);

  return overall_status ? bytes_written_total : -1;
}
