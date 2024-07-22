#ifndef STR_NUM_MSG_TRANSCODE_H
#define STR_NUM_MSG_TRANSCODE_H

#include "./protoc/str_num_msg.pb.h"

/**
 * @brief Initializes a `Num` message with a float value and a key.
 *
 * This function sets up a `Num` message structure with the provided float value
 * and key. The message is initialized to zero before setting these values.
 *
 * @param Number  The float value to be assigned to the `Num` message.
 * @param key     The key to be assigned to the `Num` message.
 * @param msg     A pointer to the `Num` message structure to be initialized.
 *
 * @note The `msg` structure is first zeroed out using `Num_init_zero` before
 * setting the `key` and `value`.
 */
void create_num(float Number, uint32_t key, Num *msg);

/**
 * @brief Initializes a `StrNum` message with a string value and a key.
 *
 * This function sets up a `StrNum` message structure with the provided string
 * value and key. The message is initialized to zero before setting these
 * values. If the string is not NULL, it sets up the necessary encoding function
 * and argument for Protocol Buffers.
 *
 * @param str     The string value to be assigned to the `StrNum` message.
 * @param key     The key to be assigned to the `StrNum` message.
 * @param msg     A pointer to the `StrNum` message structure to be initialized.
 *
 * @note The `msg` structure is first zeroed out using `StrNum_init_zero` before
 * setting the `key` and `str` values. If the string is not NULL, it prepares
 * the Protocol Buffers encoding function and argument.
 */
void create_str(const char *str, uint32_t key, Strnum *msg);

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
bool serialize_str(const Str &msg, uint8_t *buffer, size_t *buffer_size,
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
bool deserialize_str(Str &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Frees the dynamically allocated memory in a `str` message.
 *
 * This function frees any dynamically allocated memory associated with a `str`
 * message. It sets the `arg` pointer to NULL after freeing the memory to avoid
 * dangling pointers.
 *
 * @param msg  A reference to the `str` message whose memory needs to be freed.
 */
void free_str(Str &msg);

/**
 * @brief Initializes a `StrNum` message with the provided string, float, and
 * key.
 *
 * @param str Pointer to the string value for the `StrNum` message. Can be NULL.
 * @param Num The floating-point Number for the `StrNum` message.
 * @param key The unique key associated with this `StrNum` message.
 * @param msg Pointer to the `StrNum` structure to be initialized.
 */
void create_strnum(const char *str, float Num, uint32_t key, Strnum *msg);

/**
 * @brief Serializes a `Num` message into a buffer with a type ID.
 *
 * This function encodes a `Num` message into a buffer, prepending the buffer
 * with a type ID. The encoding uses Protocol Buffers (protobuf) to serialize
 * the message fields. The buffer size is updated to reflect the total size of
 * the serialized message including the type ID. The function returns a boolean
 * indicating the success or failure of the encoding operation.
 *
 * @param msg          A reference to the `Num` message to be serialized.
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
bool serialize_num(const Num &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id);

/**
 * @brief Deserializes a `Num` message from a buffer.
 *
 * This function decodes a `Num` message from a buffer. It assumes that the
 * buffer starts with a type ID followed by the serialized `Num` message data.
 * The type ID is ignored, and only the message data is used for
 * deserialization. The function returns a boolean indicating the success or
 * failure of the decoding operation.
 *
 * @param msg          A reference to the `Num` message structure where the
 * deserialized data will be stored.
 * @param buffer       A pointer to the buffer containing the serialized message
 * data with a type ID prefix.
 * @param buffer_size  The size of the buffer. This should be the total size of
 * the buffer including the type ID.
 *
 * @return `true` if deserialization is successful, `false` otherwise.
 */
bool deserialize_num(Num &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Initializes a `StrNumlst` message by associating it with a `StrNum`
 * message.
 *
 * @param strum Pointer to the `StrNum` message to include in the `StrNumlst`.
 * @param msg Pointer to the `StrNumlst` structure to be initialized.
 */
void create_strnumlst(const Strnum *strum, Strnumlist *msg);

/**
 * @brief Serializes a `StrNum` message into a byte buffer with a type
 * identifier.
 *
 * @param msg Reference to the `StrNum` message to be serialized.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to a variable containing the size of the buffer,
 * updated with the actual size of the serialized message.
 * @param type_id Type identifier to prepend to the serialized message.
 * @return true if serialization is successful, false otherwise.
 */
bool serialize_strnum(Strnum &msg, uint8_t *buffer, size_t *buffer_size,
                      uint8_t type_id);

/**
 * @brief Serializes a `StrNumlst` message into a byte buffer with a type
 * identifier.
 *
 * @param msg Pointer to the `StrNumlst` message to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Pointer to the size of the buffer; it will be updated with
 * the total size of the serialized message.
 * @param type_id The type identifier to prepend to the serialized data,
 * indicating the type of message being serialized.
 * @return true if serialization was successful, false otherwise.
 */
bool serialize_strnumlst(const Strnumlist *msg, uint8_t *buffer,
                         size_t *buffer_size, uint8_t type_id);

/**
 * @brief Deserializes a `StrNum` message from a byte buffer.
 *
 * @param msg Reference to the `StrNum` structure where the deserialized data
 * will be stored.
 * @param buffer Pointer to the byte buffer containing the serialized `StrNum`
 * message.
 * @param buffer_size Size of the buffer in bytes.
 * @return true if deserialization is successful, false otherwise.
 */
bool deserialize_strnum(Strnum &msg, const uint8_t *buffer, size_t buffer_size);

/**
 * @brief Deserializes a `StrNumlst` message from a byte buffer.
 *
 * @param msg Reference to the `StrNumlst` structure where the deserialized data
 * will be stored.
 * @param buffer Pointer to the byte buffer containing the serialized
 * `StrNumlst` message.
 * @param buffer_size Size of the buffer in bytes.
 * @return true if deserialization is successful, false otherwise.
 */
bool deserialize_strnumlst(Strnumlist &msg, const uint8_t *buffer,
                           size_t buffer_size);

/**
 * @brief Frees dynamically allocated memory in a `StrNum` message.
 *
 * This function frees any dynamically allocated memory within a `StrNum`
 * message, such as the memory allocated for the string field. It should be
 * called to prevent memory leaks when the `StrNum` message is no longer needed.
 *
 * @param msg Reference to the `StrNum` message whose dynamically allocated
 * memory needs to be freed.
 */
void free_strnum(Strnum &msg);

#endif // STR_NUM_MSG_TRANSCODE_H
