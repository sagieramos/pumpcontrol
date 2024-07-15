#include "domsgid.h"

/**
 * @brief Serializes a DoId message into a buffer.
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
                    size_t *bytes_written) {
  if (message == NULL || buffer == NULL || bytes_written == NULL) {
    *bytes_written = 0;
    return false;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

  // Encode the message
  bool status = pb_encode(&stream, DoId_fields, message);

  *bytes_written = stream.bytes_written;

  // Check if the buffer was large enough
  if (status && *bytes_written <= buffer_size) {
    return true; // Serialization successful
  } else {
    *bytes_written = 0;
    memset(buffer, 0, buffer_size);
    return false; // Buffer too small or encoding failed
  }
}

/**
 * @brief Deserializes a DoId message from a buffer.
 *
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer in bytes.
 * @param message Pointer to the DoId message structure to populate.
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_DoId(const pb_byte_t *buffer, size_t buffer_size,
                      DoId *message) {
  if (buffer == NULL || message == NULL) {
    return false;
  }
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_size);

  // Decode the message
  bool status = pb_decode(&stream, DoId_fields, message);

  // Check if the buffer was completely used and decoding was successful
  if (status && stream.bytes_left == 0) {
    return true; // Deserialization successful
  } else {
    return false; // Buffer too small or decoding failed
  }
}

/**
 * @brief Serializes a DoIdList message into a buffer.
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
                        size_t buffer_size, size_t *bytes_written) {
  // Initialize the output stream with the buffer and size
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

  // Serialize the DoIdList message
  if (!pb_encode(&stream, DoIdList_fields, message)) {
    // printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    return false; // Serialization failed
  }

  // Set the number of bytes written to the buffer
  *bytes_written = stream.bytes_written;
  return true; // Serialization successful
}

/**
 * @brief Deserializes a DoIdList message from a buffer.
 *
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param buffer_size Size of the buffer in bytes.
 * @param message Pointer to the DoIdList message structure to populate.
 * @return True if deserialization was successful, false otherwise.
 */
bool deserialize_DoIdList(const pb_byte_t *buffer, size_t buffer_size,
                          DoIdList *message) {
  // Initialize the input stream with the buffer and size
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_size);

  // Deserialize the DoIdList message
  if (!pb_decode(&stream, DoIdList_fields, message)) {
    //("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    return false; // Deserialization failed
  }

  return true; // Deserialization successful
}
