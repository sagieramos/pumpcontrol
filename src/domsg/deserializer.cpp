#include "domsg.h"

bool pb_decode_string(pb_istream_t *stream, const pb_field_t *field,
                      void **arg) {
  char *buffer = (char *)(*arg);

  // Ensure buffer is large enough for the incoming string
  if (stream->bytes_left >= strlen(buffer)) {
    return false; // Prevent buffer overflow
  }

  // Read the string from the stream
  if (!pb_read(stream, (pb_byte_t *)buffer, stream->bytes_left)) {
    return false;
  }

  // Null-terminate the string
  buffer[stream->bytes_left] = '\0';

  return true;
}

bool _deserializeDoMessage(const uint8_t *buffer, size_t buffer_len, char *inst,
                           int32_t &value, size_t &bytes_read) {
  Do pbDo = Do_init_default;

  // Create a stream that reads from the buffer
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_len);

  // Now we can decode the Do message
  bool status = pb_decode(&stream, Do_fields, &pbDo);

  if (status) {
    // Copy the inst field with null-termination
    strncpy(inst, reinterpret_cast<char *>(pbDo.inst.arg),
            sizeof(pbDo.inst.arg) - 1);
    inst[sizeof(pbDo.inst.arg) - 1] = '\0';

    // Copy the value field
    value = pbDo.value;

    bytes_read = buffer_len - stream.bytes_left; // Update the bytes read

    DEBUG_SERIAL_PRINTLN("Data deserialized successfully");
    DEBUG_SERIAL_PRINTF("Number of bytes deserialized: %zu\n", bytes_read);
    return true;
  }

  DEBUG_SERIAL_PRINTLN("Failed to deserialize data");
  DEBUG_SERIAL_PRINTLN(PB_GET_ERROR(&stream));
  return status;
}

int deserializeDoMessage(const uint8_t *buffer, size_t buffer_len,
                         DoMessage &message) {
  char inst[16];
  int32_t value = 0;
  size_t bytes_read = 0;

  if (_deserializeDoMessage(buffer, buffer_len, inst, value, bytes_read)) {
    strncpy(message.inst, inst, sizeof(message.inst) - 1);
    message.inst[sizeof(message.inst) - 1] = '\0';
    message.value = value;
    return bytes_read;
  }

  return -1;
}

int deserializeDoMessages(const uint8_t *buffer, size_t buffer_len,
                          DoMessage *messages, size_t max_messages) {
  int bytes_read_total = 0;
  size_t num_messages = 0;

  while (bytes_read_total < buffer_len && num_messages < max_messages) {
    // Read the length of the next message
    if (buffer_len - bytes_read_total < sizeof(uint32_t)) {
      DEBUG_SERIAL_PRINTLN("Not enough bytes left for length prefix");
      break;
    }

    uint32_t message_len;
    memcpy(&message_len, buffer + bytes_read_total, sizeof(uint32_t));
    bytes_read_total += sizeof(uint32_t);

    if (message_len > buffer_len - bytes_read_total) {
      DEBUG_SERIAL_PRINTLN("Message length exceeds buffer size");
      break;
    }

    char inst[16];
    int32_t value = 0;
    size_t bytes_read = 0;
    bool status = _deserializeDoMessage(buffer + bytes_read_total, message_len,
                                        inst, value, bytes_read);

    if (!status) {
      DEBUG_SERIAL_PRINTF("Failed to deserialize message %zu\n", num_messages);
      break;
    }

    strncpy(messages[num_messages].inst, inst,
            sizeof(messages[num_messages].inst) - 1);
    messages[num_messages].inst[sizeof(messages[num_messages].inst) - 1] = '\0';
    messages[num_messages].value = value;

    bytes_read_total += bytes_read;
    num_messages++;
  }

  DEBUG_SERIAL_PRINTF("Total messages deserialized: %zu\n", num_messages);
  return bytes_read_total;
}