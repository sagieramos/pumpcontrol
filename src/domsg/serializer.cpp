#include "domsg.h"

// Callback function to encode a string
bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

bool _serializeDoMessage(const char *inst, int32_t value, uint8_t *buffer,
                         size_t buffer_size, size_t &bytes_written) {
  Do pbDo = Do_init_default;

  // Set the inst field
  pbDo.inst.funcs.encode = &pb_encode_string;
  pbDo.inst.arg =
      const_cast<char *>(inst); // Set the inst field with a cast to char*

  // Set the value field
  pbDo.value = value;

  // Create a stream that will write to our buffer
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

  // Now we can encode the Do message
  bool status = pb_encode(&stream, Do_fields, &pbDo);

  if (status) {
    bytes_written = stream.bytes_written; // Update the bytes written
    DEBUG_SERIAL_PRINTLN("Data serialized successfully");
    DEBUG_SERIAL_PRINTF("Encoded size: %zu\n", stream.bytes_written);
    return true;
  }

  DEBUG_SERIAL_PRINTLN("Failed to serialize data");
  DEBUG_SERIAL_PRINTLN(PB_GET_ERROR(&stream));
  return status;
}

int serializeDoMessage(const DoMessage &message, uint8_t *buffer,
                       size_t buffer_size) {
  size_t bytes_written = 0;
  if (_serializeDoMessage(message.inst, message.value, buffer, buffer_size,
                          bytes_written)) {
    return bytes_written;
  }

  return -1;
}

int serializeDoMessage(DoMessage *messages, size_t num_messages,
                       uint8_t *buffer, size_t buffer_size) {
  int bytes_written_total = 0;
  bool overall_status = true;

  for (size_t i = 0; i < num_messages; ++i) {
    size_t bytes_written = 0;
    bool status = _serializeDoMessage(
        messages[i].inst, messages[i].value, buffer + bytes_written_total,
        buffer_size - bytes_written_total, bytes_written);

    if (!status) {
      DEBUG_SERIAL_PRINTF("Failed to serialize message %zu\n", i);
      overall_status = false;
      break;
    }

    bytes_written_total += bytes_written;
  }

  DEBUG_SERIAL_PRINTF("Total bytes written: %zu\n", bytes_written_total);

  if (overall_status)
    return bytes_written_total;

  return -1;
}
