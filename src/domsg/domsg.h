#ifndef DOMSG_H
#define DOMSG_H

#include "../dev_or_prod.h"
#include <domsg.pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

struct DoMessage {
  char inst[16];
  int32_t value;
};

/**
 * @brief Serializes a single DoMessage object into a provided buffer using
 * Protocol Buffers.
 *
 * @param message A constant reference to the DoMessage object that is to be
 * serialized.
 * @param buffer A pointer to the buffer where the serialized data will be
 * stored. This buffer should be pre-allocated and should have sufficient size
 *               to hold the serialized data.
 * @param buffer_size The size of the provided buffer in bytes.
 *                    This specifies the maximum amount of space available for
 * the serialized data.
 * @return int Returns the number of bytes written to the buffer on successful
 * serialization. If the serialization fails, it returns a negative value
 * indicating the error.
 */
int serializeDoMessage(const DoMessage &message, uint8_t *buffer,
                       size_t buffer_size);

/**
 * @brief Serializes an array of DoMessage objects into a provided buffer using
 * Protocol Buffers.
 *
 * @param messages A pointer to the array of DoMessage objects to be serialized.
 * @param num_messages The number of DoMessage objects in the array.
 * @param buffer A pointer to the buffer where the serialized data will be
 * stored. This buffer should be pre-allocated and should have sufficient size
 *               to hold the serialized data.
 * @param buffer_size The size of the provided buffer in bytes.
 *                    This specifies the maximum amount of space available for
 * the serialized data.
 * @return int Returns the total number of bytes written to the buffer on
 * successful serialization. If the serialization fails, it returns a negative
 * value indicating the error.
 */
int serializeDoMessage(DoMessage *messages, size_t num_messages,
                       uint8_t *buffer, size_t buffer_size);

#endif
