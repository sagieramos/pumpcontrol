#ifndef DOSGID_H
#define DOSGID_H

#include "dev_or_prod.h"
#include <domsgid.pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#define MAX_ARR_LEN 100

/**
 * @brief Serializes a single DoId object into a provided buffer using
 * Protocol Buffers.
 *
 * This function converts the individual fields of the DoId object into
 * a binary format and writes it to the provided buffer.
 *
 * @param buffer A pointer to the buffer where the serialized data will
 *               be stored. This buffer should be pre-allocated and should
 *               have sufficient size to hold the serialized data.
 * @param buffer_size The size of the provided buffer in bytes. This specifies
 *                    the maximum amount of space available for the serialized
 *                    data.
 * @param id The integer ID to be serialized.
 * @param value The integer value to be serialized.
 * @param bytes_written A reference to a size_t variable that will store the
 *                      number of bytes written to the buffer during
 * serialization.
 * @return bool Returns true if serialization is successful; otherwise, returns
 *              false indicating an error occurred during the serialization
 * process.
 */
int serialize_do_id(uint8_t *buffer, size_t buffer_size, const DoId &obj);

/**
 * @brief Serializes a single DoId object into a provided buffer using
 * Protocol Buffers.
 *
 * This function verifies the input parameters, and if valid, serializes the
 * provided DoId object into the buffer.
 *
 * @param buffer A pointer to the buffer where the serialized data will be
 *               stored. This buffer should be pre-allocated and should
 *               have sufficient size to hold the serialized data.
 * @param buffer_size The size of the provided buffer in bytes. This specifies
 *                    the maximum amount of space available for the serialized
 *                    data.
 * @param obj A constant reference to the DoId object that is to be serialized.
 * @return int Returns the number of bytes written to the buffer on successful
 *              serialization. If the serialization fails, it returns a negative
 *              value indicating the error.
 */
int serialize_do_id(uint8_t *buffer, size_t buffer_size, const DoId *objs,
                    const size_t max_objs);
/**
 * @brief Serializes multiple DoId objects into a provided buffer using
 * Protocol Buffers.
 *
 * This function serializes an array of DoId objects and writes the binary
 * representation into the provided buffer. It stops serializing upon reaching
 * the end of the buffer or the maximum number of objects.
 *
 * @param buffer A pointer to the buffer where the serialized data will be
 *               stored. This buffer should be pre-allocated and should
 *               have sufficient size to hold the serialized data.
 * @param buffer_size The size of the provided buffer in bytes. This specifies
 *                    the maximum amount of space available for the serialized
 *                    data.
 * @param objs A pointer to an array of DoId objects that are to be serialized.
 *             This array must not be null and should contain valid DoId
 * objects.
 * @param num_objs The number of DoId objects in the objs array that will
 *                 be serialized.
 * @return int Returns the total number of bytes written to the buffer on
 *              successful serialization of all objects. If the serialization
 *              fails for any object or if input parameters are invalid,
 *              it returns a negative value indicating the error.
 */

/**
 * @brief Deserializes a single DoId message from the provided buffer.
 *
 * This function converts the binary representation of a DoId message back
 * into a usable DoId object. It verifies input parameters and logs errors
 * as needed during the deserialization process.
 *
 * @param buffer A pointer to the byte buffer containing the serialized
 *               DoId message. This buffer must not be null.
 * @param buffer_size The size of the buffer in bytes. This must be greater
 *                    than zero to ensure valid deserialization.
 * @param obj A reference to a DoId object where the deserialized data will
 *            be stored. This object must be properly initialized.
 * @return int Returns the size of the buffer on successful deserialization.
 *             Returns -1 if the input parameters are invalid or if
 *             deserialization fails.
 */
int deserialize_do_id(const uint8_t *buffer, size_t buffer_size, DoId &obj);

/**
 * @brief Deserializes multiple DoId messages from the provided buffer.
 *
 * This function reads sequentially from the buffer and populates an array
 * of DoId objects with the deserialized data. It stops processing if the
 * buffer is exhausted or the maximum number of objects is reached.
 *
 * @param buffer A pointer to the byte buffer containing the serialized
 *               DoId messages. This buffer must not be null.
 * @param buffer_size The size of the buffer in bytes. This must be greater
 *                    than zero to ensure valid deserialization.
 * @param bytes_read_total A reference to a variable that will be updated
 *                         with the total number of bytes read from the buffer.
 * @param objs A pointer to an array of DoId objects where the deserialized
 *             messages will be stored. This array must not be null and
 *             should have sufficient capacity.
 * @param max_objs The maximum number of DoId objects that can be filled in
 *                 the objs array. Defaults to MAX_ARR_LEN.
 * @return int Returns the number of DoId messages successfully deserialized.
 *             Returns -1 if the input parameters are invalid or if
 *             deserialization fails.
 */
int deserialize_do_id(const uint8_t *buffer, size_t buffer_size,
                      size_t &bytes_read_total, DoId *objs,
                      size_t max_objs = MAX_ARR_LEN);

#endif