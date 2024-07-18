#ifndef STR_NUM_MSG_TRANSCODE_H
#define STR_NUM_MSG_TRANSCODE_H

#include <str_num_msg.pb.h>

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
