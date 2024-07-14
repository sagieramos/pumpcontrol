#ifndef DOMSGID_H
#define DOMSGID_H

#include "domsgid.pb.h" // Path to the generated protobuf header
#include <pb_decode.h>
#include <pb_encode.h>

/**
 * @file domsgid.h
 * @brief Functions for serializing and deserializing DoId and DoIdList
 * messages.
 *
 * This header file provides the declarations for functions that handle the
 * serialization and deserialization of DoId and DoIdList messages using
 * Protocol Buffers. The functions convert these messages to and from a binary
 * format suitable for network transmission or storage.
 */

/**
 * @brief Serializes a DoId message into a buffer.
 *
 * This function encodes a DoId message into a binary format suitable for
 * network transmission or storage. The buffer provided must be large enough to
 * hold the serialized data.
 *
 * @param message Pointer to the DoId message to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Size of the buffer in bytes.
 * @param bytes_written Pointer to store the number of bytes written to the
 * buffer.
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_DoId(const DoId *message, pb_byte_t *buffer, size_t buffer_size,
                    size_t *bytes_written);

/**
 * @brief Deserializes a DoId message from a buffer.
 *
 * This function decodes a DoId message from a binary format into the message
 * structure. The buffer must contain valid serialized data and the size must
 * match the length of the serialized message.
 *
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer in bytes.
 * @param message Pointer to the DoId message structure to populate.
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_DoId(const pb_byte_t *buffer, size_t buffer_size,
                      DoId *message);

/**
 * @brief Serializes a DoIdList message into a buffer.
 *
 * This function encodes a DoIdList message into a binary format suitable for
 * network transmission or storage. The buffer provided must be large enough to
 * hold the serialized data.
 *
 * @param message Pointer to the DoIdList message to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be
 * written.
 * @param buffer_size Size of the buffer in bytes.
 * @param bytes_written Pointer to store the number of bytes written to the
 * buffer.
 * @return True if serialization was successful, false otherwise.
 */
bool serialize_DoIdList(const DoIdList *message, pb_byte_t *buffer,
                        size_t buffer_size, size_t *bytes_written);

/**
 * @brief Deserializes a DoIdList message from a buffer.
 *
 * This function decodes a DoIdList message from a binary format into the
 * message structure. The buffer must contain valid serialized data and the size
 * must match the length of the serialized message.
 *
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer in bytes.
 * @param message Pointer to the DoIdList message structure to populate.
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_DoIdList(const pb_byte_t *buffer, size_t buffer_size,
                          DoIdList *message);

#endif // DOMSGID_H
