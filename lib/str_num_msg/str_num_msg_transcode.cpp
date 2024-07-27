#include "./str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdlib.h>
#include <string.h>

bool deserialize_stringlist(Stringlist &msg, const uint8_t *buffer, size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  msg.strs.funcs.decode = &pb_decode_stringlist;
  return pb_decode(&stream, Stringlist_fields, &msg);
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

bool serialize_stringlist(Stringlist &msg, uint8_t *buffer, size_t *buffer_size,
                       uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
    msg.strs.funcs.encode = &encode_string;

  bool status = pb_encode(&stream, Stringlist_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
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
  msg.strnums.funcs.decode = &pb_decode_strnumlist;
  return pb_decode(&stream, Strnumlist_fields, &msg);
}
