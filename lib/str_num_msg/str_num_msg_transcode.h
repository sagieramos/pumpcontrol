#ifndef STR_NUM_MSG_TRANSCODE_H
#define STR_NUM_MSG_TRANSCODE_H

#include "./protoc/str_num_msg.pb.h"

/**
 * @brief Initializes a `num` message with a float value and a key.
 *
 * This function sets up a `num` message structure with the provided float value
 * and key. The message is initialized to zero before setting these values.
 *
 * @param number  The float value to be assigned to the `num` message.
 * @param key     The key to be assigned to the `num` message.
 * @param msg     A pointer to the `num` message structure to be initialized.
 *
 * @note The `msg` structure is first zeroed out using `num_init_zero` before
 * setting the `key` and `value`.
 */
void create_num(float number, uint32_t key, num *msg);

/**
 * @brief Initializes a `strnum` message with a string value and a key.
 *
 * This function sets up a `strnum` message structure with the provided string
 * value and key. The message is initialized to zero before setting these
 * values. If the string is not NULL, it sets up the necessary encoding function
 * and argument for Protocol Buffers.
 *
 * @param str     The string value to be assigned to the `strnum` message.
 * @param key     The key to be assigned to the `strnum` message.
 * @param msg     A pointer to the `strnum` message structure to be initialized.
 *
 * @note The `msg` structure is first zeroed out using `strnum_init_zero` before
 * setting the `key` and `str` values. If the string is not NULL, it prepares
 * the Protocol Buffers encoding function and argument.
 */
void create_str(const char *str, uint32_t key, strnum *msg);

/**
 * @brief Serializes a `str` message into a buffer with a type ID.
 *
 * This function encodes a `str` message into a buffer, prepending the buffer
 * with a type ID. The encoding uses Protocol Buffers (protobuf) to serialize
 * the message fields. The buffer size is updated to reflect the total size of
 * the serialized message including the type ID. The function returns a boolean
 * indicating the success or failure of the encoding operation.
 *
 * @param msg          A reference to the `str` message to be serialized.
 * @param buffer       A pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size  A pointer to the size of the buffer. On entry, it should
 * contain the maximum size of the buffer. On exit, it will contain the size of
 * the serialized data including the type ID.
 * @param type_id      A type identifier that will be written as the first byte
 * of the buffer.
 *
 * @return `true` if serialization is successful, `false` otherwise.
 */
bool serialize_str(const str &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id);

/**
 * @brief Deserializes a `str` message from a buffer.
 *
 * This function decodes a `str` message from a buffer. It assumes that the
 * buffer starts with a type ID followed by the serialized `str` message data.
 * The type ID is ignored, and only the message data is used for
 * deserialization. The function returns a boolean indicating the success or
 * failure of the decoding operation.
 *
 * @param msg          A reference to the `str` message structure where the
 * deserialized data will be stored.
 * @param buffer       A pointer to the buffer containing the serialized message
 * data with a type ID prefix.
 * @param buffer_size  The size of the buffer. This should be the total size of
 * the buffer including the type ID.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */
bool deserialize_str(str &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Frees the dynamically allocated memory in a `str` message.
 *
 * This function frees any dynamically allocated memory associated with a `str`
 * message. It sets the `arg` pointer to NULL after freeing the memory to avoid
 * dangling pointers.
 *
 * @param msg  A reference to the `str` message whose memory needs to be freed.
 */
void free_str(str &msg);

/**
 * @brief Initializes a `strnum` message with the provided string, float, and
 * key.
 *
 * @param str Pointer to the string value for the `strnum` message. Can be NULL.
 * @param num The floating-point number for the `strnum` message.
 * @param key The unique key associated with this `strnum` message.
 * @param msg Pointer to the `strnum` structure to be initialized.
 */
void create_strunum(const char *str, float num, uint32_t key, strnum *msg);

/**
 * @brief Serializes a `num` message into a buffer with a type ID.
 *
 * This function encodes a `num` message into a buffer, prepending the buffer
 * with a type ID. The encoding uses Protocol Buffers (protobuf) to serialize
 * the message fields. The buffer size is updated to reflect the total size of
 * the serialized message including the type ID. The function returns a boolean
 * indicating the success or failure of the encoding operation.
 *
 * @param msg          A reference to the `num` message to be serialized.
 * @param buffer       A pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size  A pointer to the size of the buffer. On entry, it should
 * contain the maximum size of the buffer. On exit, it will contain the size of
 * the serialized data including the type ID.
 * @param type_id      A type identifier that will be written as the first byte
 * of the buffer.
 *
 * @return `true` if serialization is successful, `false` otherwise.
 */
bool serialize_num(const num &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id);

/**
 * @brief Deserializes a `num` message from a buffer.
 *
 * This function decodes a `num` message from a buffer. It assumes that the
 * buffer starts with a type ID followed by the serialized `num` message data.
 * The type ID is ignored, and only the message data is used for
 * deserialization. The function returns a boolean indicating the success or
 * failure of the decoding operation.
 *
 * @param msg          A reference to the `num` message structure where the
 * deserialized data will be stored.
 * @param buffer       A pointer to the buffer containing the serialized message
 * data with a type ID prefix.
 * @param buffer_size  The size of the buffer. This should be the total size of
 * the buffer including the type ID.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */
bool deserialize_num(num &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Initializes a `strnumlst` message by associating it with a `strnum`
 * message.
 *
 * @param strum Pointer to the `strnum` message to include in the `strnumlst`.
 * @param msg Pointer to the `strnumlst` structure to be initialized.
 */
void create_strnumlst(const strnum *strum, strnumlst *msg);

/**
 * @brief Serializes a `strnum` message into a byte buffer with a type
 * identifier.
 *
 * @param msg Reference to the `strnum` message to be serialized.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to a variable containing the size of the buffer,
 * updated with the actual size of the serialized message.
 * @param type_id Type identifier to prepend to the serialized message.
 * @return true if serialization is successful, false otherwise.
 */
bool serialize_strnum(strnum &msg, uint8_t *buffer, size_t *buffer_size,
                      uint8_t type_id);

/**
 * @brief Serializes a `strnumlst` message into a byte buffer with a type
 * identifier.
 *
 * @param msg Pointer to the `strnumlst` message to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to the size of the buffer; it will be updated with
 * the total size of the serialized message.
 * @param type_id The type identifier to prepend to the serialized data,
 * indicating the type of message being serialized.
 * @return true if serialization was successful, false otherwise.
 */
bool serialize_strnumlst(const strnumlst *msg, uint8_t *buffer,
                         size_t *buffer_size, uint8_t type_id);

/**
 * @brief Deserializes a `strnum` message from a byte buffer.
 *
 * @param msg Reference to the `strnum` structure where the deserialized data
 * will be stored.
 * @param buffer Pointer to the byte buffer containing the serialized `strnum`
 * message.
 * @param buffer_size Size of the buffer in bytes.
 * @return true if deserialization is successful, false otherwise.
 */
bool deserialize_strnum(strnum &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Deserializes a `strnumlst` message from a byte buffer.
 *
 * @param msg Reference to the `strnumlst` structure where the deserialized data
 * will be stored.
 * @param buffer Pointer to the byte buffer containing the serialized
 * `strnumlst` message.
 * @param buffer_size Size of the buffer in bytes.
 * @return true if deserialization is successful, false otherwise.
 */
bool deserialize_strnumlst(strnumlst &msg, const uint8_t *buffer,
                           size_t buffer_size);

/**
 * @brief Frees dynamically allocated memory in a `strnum` message.
 *
 * This function frees any dynamically allocated memory within a `strnum`
 * message, such as the memory allocated for the string field. It should be
 * called to prevent memory leaks when the `strnum` message is no longer needed.
 *
 * @param msg Reference to the `strnum` message whose dynamically allocated
 * memory needs to be freed.
 */
void free_strnum(strnum &msg);

#endif // STR_NUM_MSG_TRANSCODE_H
