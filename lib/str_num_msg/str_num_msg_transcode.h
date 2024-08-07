#ifndef STR_NUM_MSG_TRANSCODE_H
#define STR_NUM_MSG_TRANSCODE_H

#include "./protoc/str_num_msg.pb.h"

/**
 * @brief Type definition for a callback function used in
 * serialization/deserialization.
 *
 * @param data Pointer to the data buffer.
 * @param len Length of the data buffer.
 */
typedef void (*strnum_codec_callback_t)(void *data, size_t len);

/**
 * @brief callback function to encode a string.
 *
 * @param stream The stream to write the encoded data to.
 * @param field The field descriptor for the field being encoded.
 * @param arg Pointer to the string to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
                   void *const *arg);

/**
 * @brief callback function to encode a Strnum message.
 *
 * @param stream The stream to write the encoded data to.
 * @param field The field descriptor for the field being encoded.
 * @param arg Pointer to the Strnum message to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool pb_encode_strnum(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg);

/**
 * @brief callback function to decode a string from a buffer.
 *
 * @param stream The stream to read the encoded data from.
 * @param field The field descriptor for the field being decoded.
 * @param arg Pointer to the pointer where the decoded string will be stored.
 *
 * @return True if decoding was successful, false otherwise.
 */
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

// Functions for creating messages

/**
 * @brief Initializes a Num message with given values.
 *
 * @param number The float value to set in the message.
 * @param key The key to set in the message.
 * @param msg The Num message to initialize.
 */
void create_num(float number, uint32_t key, Num &msg);

/**
 * @brief Initializes a Str message with a given string and key.
 *
 * @param str The string to set in the message.
 * @param key The key to set in the message.
 * @param msg The Str message to initialize.
 */
void create_str(const char *str, uint32_t key, Str &msg);

/**
 * @brief Initializes a Strnum message with a string, a float number, and a key.
 *
 * @param str The string to set in the message.
 * @param num The float number to set in the message.
 * @param key The key to set in the message.
 * @param msg The Strnum message to initialize.
 */
void create_strnum(const char *str, float num, uint32_t key, Strnum &msg);

/**
 * @brief Initializes an Auth message with the provided username and password.
 *
 * @param username The username to set in the message.
 * @param password The password to set in the message.
 * @param msg Pointer to the Auth message to initialize.
 */
void create_auth(const char *id, const char *pass, Auth &msg);

/**
 * @brief Serializes a Num message into a buffer.
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
                   uint8_t type_id, strnum_codec_callback_t cb = NULL);

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
                     strnum_codec_callback_t cb = NULL);

/**
 * @brief Serializes a Str message into a buffer.
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
                   uint8_t type_id, strnum_codec_callback_t cb = NULL);

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
                     strnum_codec_callback_t cb = NULL);

/**
 * @brief Serializes a Strnum message into a buffer.
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
                      uint8_t type_id, strnum_codec_callback_t cb = NULL);

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
                        strnum_codec_callback_t cb = NULL);

// Free memory for Strnum message
/**
 * @brief Frees memory allocated for a Strnum message.
 *
 * @param msg The Strnum message to free.
 *
 * @return void
 */
void free_strnum(Strnum &msg);

// Free memory for Str message
/**
 * @brief Frees memory allocated for a Str message.
 *
 * @param msg The Str message to free.
 *
 * @return void
 */
void free_str(Str &msg);

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
                    uint8_t type_id, strnum_codec_callback_t cb);

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
                      strnum_codec_callback_t cb = NULL);

/* callback function to encode a stringlist message
 * @param stream The stream to write to.
 * @param field The field descriptor for the stringlist message.
 * @param arg The stringlist message to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */

/**
 * @brief callback function to decode a string from a protobuf message.
 *
 * @param stream The stream to read from.
 * @param field The field descriptor for the string.
 * @param arg Pointer to where the decoded string should be stored.
 *
 * @return True if decoding was successful, false otherwise.
 */
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

/**
 * @brief Frees memory allocated for an Auth message.
 *
 * @param msg The Auth message to free.
 */
void free_auth(Auth &msg);

#endif