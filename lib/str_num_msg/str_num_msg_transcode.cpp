#include "./str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdlib.h>
#include <string.h>

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
  msg.value.funcs.decode = &decode_string;

  bool status = pb_decode(&stream, Str_fields, &msg);
  if (status && cb) {
    cb((void *)&msg, buffer_size);
    free_str(msg);
  }
  return status;
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
  msg.str.funcs.decode = &decode_string;

  bool status = pb_decode(&stream, Strnum_fields, &msg);
  if (status && cb) {
    cb(static_cast<void *>(&msg), buffer_size);
    free_strnum(msg);
  }

  return status;
}

// Serialize Auth message
/**
 * @brief Serializes an Auth message to a buffer.
 *
 * @param msg The Auth message to serialize.
 * @param buffer The buffer to store the serialized data.
 * @param buffer_size Pointer to the size of the buffer.
 * @param type_id The type ID to prefix the message with.
 * @param cb Optional callback function to call after serialization.
 *
 * @return True if serialization was successful, false otherwise.
 */

bool serialize_auth(const Auth &msg, uint8_t *buffer, size_t *buffer_size,
                    uint8_t type_id, strnum_codec_callback_t cb) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Auth_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
    if (cb) {
      cb((void *)buffer, *buffer_size);
    }
  }
  return status;
}

// Deserialize Auth message
/**
 * @brief Deserializes an Auth message from a buffer.
 *
 * @param msg The Auth message to deserialize into.
 * @param buffer The buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 * @param cb Optional callback function to call after deserialization.
 *
 * @return True if deserialization was successful, false otherwise.
 */

bool deserialize_auth(Auth &msg, const uint8_t *buffer, size_t buffer_size,
                      strnum_codec_callback_t cb) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  msg.id.funcs.decode = &decode_string;
  msg.pass.funcs.decode = &decode_string;

  bool status = pb_decode(&stream, Auth_fields, &msg);
  if (status && cb) {
    cb(static_cast<void *>(&msg), buffer_size);
    free_auth(msg);
  }

  return status;
}
