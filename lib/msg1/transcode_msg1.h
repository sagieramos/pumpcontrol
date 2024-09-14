#ifndef TRANSCODE_MSG1_H
#define TRANSCODE_MSG1_H

#include "./protoc/msg1.pb.h"

typedef void (*msg1_codec_callback_t)(void *data, size_t len);

/**
 * @brief Serializes a `msg1` message into a buffer with a type ID.
 *
 * This function encodes a `msg1` message into a byte buffer, prepending
 * the buffer with a type ID. The encoding uses Protocol Buffers (protobuf) to
 * serialize the message fields. The buffer size is updated to reflect the total
 * size of the serialized message including the type ID.
 *
 * @param msg1 The `msg1` structure to be serialized.
 * @param buffer A pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size A pointer to the size of the buffer. On entry, it should
 * contain the maximum size of the buffer. On exit, it will contain the size of
 * the serialized data including the type ID.
 * @param type_id A type identifier that will be written as the first byte of
 * the buffer.
 * @param cb Optional callback function to call with the serialized data.
 *
 * @return `true` if serialization is successful, `false` otherwise.
 */

bool serialize_msg1(const Msg1 &msg1, uint8_t *buffer, size_t *buffer_size,
                    uint8_t type_id, msg1_codec_callback_t cb = nullptr);

/**
 * @brief Deserializes a `msg1` message from a buffer.
 *
 * This function decodes a `msg1` message from a byte buffer. The buffer is
 * expected to be prepended with a type ID. The decoding uses Protocol Buffers
 * (protobuf) to deserialize the message fields.
 *
 * @param buffer A pointer to the buffer containing the serialized data.
 * @param buffer_size The size of the buffer.
 * @param msg1 The `msg1` structure where the deserialized data will be written.
 * @param cb Optional callback function to call with the deserialized data.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */

bool deserialize_msg1(const uint8_t *buffer, size_t buffer_size, Msg1 &msg1,
                      msg1_codec_callback_t cb = nullptr);

#endif // TRANSCODE_MSG1_H