#include "./str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdlib.h>
#include <string.h>

// callback function to encode a string
/**
 * @brief callback function to encode a string in a protobuf message.
 *
 * @param stream The stream to write to.
 * @param field The field descriptor for the string.
 * @param arg The string to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

// callback function to encode a Strnum message
/**
 * @brief callback function to encode a Strnum message in a protobuf
 * message.
 *
 * @param stream The stream to write to.
 * @param field The field descriptor for the Strnum message.
 * @param arg The Strnum message to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool pb_encode_strnum(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const Strnum *msg = (const Strnum *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_submessage(stream, Strnum_fields, msg);
}

// callback function to decode a string
/**
 * @brief callback function to decode a string from a protobuf message.
 *
 * @param stream The stream to read from.
 * @param field The field descriptor for the string.
 * @param arg Pointer to where the decoded string should be stored.
 *
 * @return True if decoding was successful, false otherwise.
 */
bool pb_decode_string(pb_istream_t *stream, const pb_field_t *field,
                      void **arg) {
  size_t str_len = stream->bytes_left;
  char *str = (char *)malloc(str_len + 1);
  if (!str) {
    return false;
  }

  if (!pb_read(stream, (pb_byte_t *)str, str_len)) {
    free(str);
    return false;
  }
  str[str_len] = '\0';
  *arg = str;
  return true;
}

// Create Num message
/**
 * @brief Initializes a Num message with the provided values.
 *
 * @param number The float value to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Num message to initialize.
 */
void create_num(float number, uint32_t key, Num &msg) {
  msg = Num_init_zero;
  msg.key = key;
  msg.value = number;
}

// Create Str message
/**
 * @brief Initializes a Str message with the provided string and key.
 *
 * @param str The string to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Str message to initialize.
 */
void create_str(const char *str, uint32_t key, Str &msg) {
  msg = Str_init_zero;
  msg.key = key;
  if (str != NULL) {
    msg.value.funcs.encode = &pb_encode_string;
    msg.value.arg = (void *)str;
  }
}

// Create Strnum message
/**
 * @brief Initializes a Strnum message with the provided string, number, and
 * key.
 *
 * @param str The string to set in the message.
 * @param num The float value to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Strnum message to initialize.
 */
void create_strnum(const char *str, float num, uint32_t key, Strnum &msg) {
  msg = Strnum_init_zero;
  msg.key = key;
  msg.num = num;
  if (str != NULL) {
    msg.str.funcs.encode = &pb_encode_string;
    msg.str.arg = (void *)str;
  }
}

// Create Strnumlist message
/**
 * @brief Initializes a Strnumlist message with the provided Strnum data.
 *
 * @param strum The Strnum data to set in the message.
 * @param msg Pointer to the Strnumlist message to initialize.
 */
void create_strnumlst(const Strnum *strum, Strnumlist &msg) {
  msg = Strnumlist_init_zero;
  msg.str_nums.funcs.encode = &pb_encode_strnum;
  msg.str_nums.arg = (void *)strum;
}

// Serialize Num message
/**
 * @brief Serializes a Num message to a buffer.
 *
 * @param msg The Num message to serialize.
 * @param buffer The buffer to store the serialized data.
 * @param buffer_size Pointer to the size of the buffer.
 * @param type_id The type ID to prefix the message with.
 * @param cb Optional callback function to call after serialization.
 *
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_num(const Num &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id, strnum_codec_callback_t cb) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Num_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
    if (cb) {
      cb((void *)buffer, *buffer_size);
    }
  }
  return status;
}

// Deserialize Num message
/**
 * @brief Deserializes a Num message from a buffer.
 *
 * @param msg The Num message to deserialize into.
 * @param buffer The buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 * @param cb Optional callback function to call after deserialization.
 *
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_num(Num &msg, const uint8_t *buffer, size_t buffer_size,
                     strnum_codec_callback_t cb) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  bool status = pb_decode(&stream, Num_fields, &msg);
  if (status && cb) {
    cb((void *)buffer, buffer_size);
  }
  return status;
}

// Serialize Str message
/**
 * @brief Serializes a Str message to a buffer.
 *
 * @param msg The Str message to serialize.
 * @param buffer The buffer to store the serialized data.
 * @param buffer_size Pointer to the size of the buffer.
 * @param type_id The type ID to prefix the message with.
 * @param cb Optional callback function to call after serialization.
 *
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_str(const Str &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id, strnum_codec_callback_t cb) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Str_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
    if (cb) {
      cb((void *)buffer, *buffer_size);
    }
  }
  return status;
}

// Deserialize Str message
/**
 * @brief Deserializes a Str message from a buffer.
 *
 * @param msg The Str message to deserialize into.
 * @param buffer The buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 * @param cb Optional callback function to call after deserialization.
 *
 * @return True if deserialization was successful, false otherwise.
 */

bool deserialize_str(Str &msg, const uint8_t *buffer, size_t buffer_size,
                     strnum_codec_callback_t cb) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  msg.value.funcs.decode = &pb_decode_string;

  bool status = pb_decode(&stream, Str_fields, &msg);
  if (status && cb) {
    cb((void *)&msg, buffer_size);
    free_str(msg);
  }
  return status;
}

// Free memory for Str message
/**
 * @brief Frees memory allocated for a Str message.
 *
 * @param msg The Str message to free.
 */
void free_str(Str &msg) {
  if (msg.value.arg) {
    free(msg.value.arg);
    msg.value.arg = NULL;
  }
}

// Serialize Strnum message
/**
 * @brief Serializes a Strnum message to a buffer.
 *
 * @param msg The Strnum message to serialize.
 * @param buffer The buffer to store the serialized data.
 * @param buffer_size Pointer to the size of the buffer.
 * @param type_id The type ID to prefix the message with.
 * @param cb Optional callback function to call after serialization.
 *
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_strnum(Strnum &msg, uint8_t *buffer, size_t *buffer_size,
                      uint8_t type_id, strnum_codec_callback_t cb) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Strnum_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
    if (cb) {
      cb((void *)buffer, *buffer_size);
    }
  }
  return status;
}

// Deserialize Strnum message
/**
 * @brief Deserializes a Strnum message from a buffer.
 *
 * @param msg The Strnum message to deserialize into.
 * @param buffer The buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 * @param cb Optional callback function to call after deserialization.
 *
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_strnum(Strnum &msg, const uint8_t *buffer, size_t buffer_size,
                        strnum_codec_callback_t cb) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  msg.str.funcs.decode = &pb_decode_string;

  bool status = pb_decode(&stream, Strnum_fields, &msg);
  if (status && cb) {
    cb(static_cast<void *>(&msg), buffer_size);
    free_strnum(msg);
  }

  return status;
}

// Free memory for Strnum message
/**
 * @brief Frees memory allocated for a Strnum message.
 *
 * @param msg The Strnum message to free.
 */
void free_strnum(Strnum &msg) {
  if (msg.str.arg) {
    free(msg.str.arg);
    msg.str.arg = NULL;
  }
}

// Serialize Strnumlist message
/**
 * @brief Serializes a Strnumlist message to a buffer.
 *
 * @param msg The Strnumlist message to serialize.
 * @param buffer The buffer to store the serialized data.
 * @param buffer_size Pointer to the size of the buffer.
 * @param type_id The type ID to prefix the message with.
 *
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_strnumlst(const Strnumlist *msg, uint8_t *buffer,
                         size_t *buffer_size, uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Strnumlist_fields, msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

// Deserialize Strnumlist message
/**
 * @brief Deserializes a Strnumlist message from a buffer.
 *
 * @param msg The Strnumlist message to deserialize into.
 * @param buffer The buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 *
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_strnumlst(Strnumlist &msg, const uint8_t *buffer,
                           size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, Strnumlist_fields, &msg);
}
