#include "main.h"

// Buffer to hold the deserialized string
char deserialized_inst[MAX_BUFFER_SIZE / 2];

uint32_t getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  uint32_t currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}

// Callback function to encode a string
bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (uint8_t *)str, strlen(str));
}

bool pb_decode_string(pb_istream_t *stream, const pb_field_t *field,
                      void **arg) {
  // Read the string from the stream
  if (!pb_read(stream, (pb_byte_t *)deserialized_inst, stream->bytes_left)) {
    return false;
  }
  // Null-terminate the string
  deserialized_inst[stream->bytes_left] = '\0';
  *arg = deserialized_inst;
  return true;
}

bool serializeDoMessage(const char *inst, int32_t value, uint8_t *buffer,
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

//(uint8_t *buffer, DoMessage *messages, size_t num_messages, size_t
// buffer_size, size_t &bytes_written_total)

bool serializeDoMessage(uint8_t *buffer, DoMessage &obj, size_t buffer_size,
                        size_t &bytes_written) {
  return serializeDoMessage(obj.inst, obj.value, buffer, buffer_size,
                            bytes_written);
}

bool deserializeDoMessage(const uint8_t *buffer, DoMessage &objToFill,
                          size_t buffer_len) {
  Do pbDo = Do_init_default;

  // Set the callback function for the inst field
  pbDo.inst.funcs.decode = pb_decode_string;
  pbDo.inst.arg = deserialized_inst;

  // Create a stream that reads from the buffer
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_len);

  // Decode the incoming message
  bool status = pb_decode(&stream, Do_fields, &pbDo);

  if (status) {
    // Fill the objToFill with deserialized data
    strncpy(objToFill.inst, deserialized_inst, sizeof(objToFill.inst));
    objToFill.value = pbDo.value;

    Serial.println("Data deserialized successfully");
    Serial.print("inst: ");
    Serial.println(objToFill.inst);
    Serial.print("value: ");
    Serial.println(objToFill.value);
  } else {
    Serial.println("Failed to deserialize data");
    Serial.println(PB_GET_ERROR(&stream));
  }

  return status;
}

int serializeArrayDoMessage(uint8_t *buffer, DoMessage *messages,
                            size_t num_messages, size_t buffer_size) {
  int bytes_written_total = 0;
  bool overall_status = true;

  for (size_t i = 0; i < num_messages; ++i) {
    size_t bytes_written = 0;
    bool status = serializeDoMessage(
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

bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
                   void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

bool serializeDoMessage(uint8_t *buffer, DoMessage *messages,
                        size_t num_messages, size_t buffer_size,
                        size_t &bytes_written_total) {
  bytes_written_total = 0;
  bool overall_status = true;

  for (size_t i = 0; i < num_messages; ++i) {
    pb_ostream_t stream = pb_ostream_from_buffer(
        buffer + bytes_written_total, buffer_size - bytes_written_total);

    Do pbDo = Do_init_default;
    pbDo.inst.funcs.encode = &encode_string;
    pbDo.inst.arg = (void *)messages[i].inst;
    pbDo.value = messages[i].value;

    bool status = pb_encode(&stream, Do_fields, &pbDo);

    if (!status) {
      DEBUG_SERIAL_PRINTF("Failed to serialize message %zu: %s\n", i,
                          PB_GET_ERROR(&stream));
      overall_status = false;
      break;
    }

    bytes_written_total += stream.bytes_written;
  }

  DEBUG_SERIAL_PRINTF("Total bytes written: %zu\n", bytes_written_total);

  return overall_status;
}

// Using bool serializeDoMessage(uint8_t *buffer, DoMessage *messages, size_t
// num_messages, size_t buffer_size) example

/*     const size_t NUM_MESSAGES = 3;
    DoMessage messages[NUM_MESSAGES] = {
        {"inst1", 10},
        {"inst2", 20},
        {"inst3", 30}
    };

    const size_t BUFFER_SIZE = NUM_MESSAGES * sizeof(Do);  // Adjust size based
   on your actual Do message size uint8_t buffer[BUFFER_SIZE];

    // Serialize the array of messages
    bool success = serializeDoMessage(buffer, messages, NUM_MESSAGES,
   BUFFER_SIZE);

    if (success) {
        // Serialization was successful
        Serial.println("Serialization successful!");

        // Now you can use the serialized data in `buffer`
        // For example, you could send `buffer` over a network, write it to a
   file, etc. } else {
        // Serialization failed
        Serial.println("Serialization failed!");
        // Handle the failure as needed
    } */