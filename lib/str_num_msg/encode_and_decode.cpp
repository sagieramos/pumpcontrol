#include "./str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdlib.h>
#include <string.h>

// callback function to encode a string
/**
 * @brief callback function to encode a string in a protobuf message.
 *
 * @param stream The stream to write to.
 * @param field The field descriptor for the string.
 * @param arg The string to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
                   void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

// callback function to decode a string
/**
 * @brief callback function to decode a string from a protobuf message.
 *
 * @param stream The stream to read from.
 * @param field The field descriptor for the string.
 * @param arg Pointer to where the decoded string should be stored.
 *
 * @return True if decoding was successful, false otherwise.
 */
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  size_t str_len = stream->bytes_left;
  char *str = (char *)malloc(str_len + 1);
  if (!str) {
    return false;
  }

  if (!pb_read(stream, (pb_byte_t *)str, str_len)) {
    free(str);
    return false;
  }
  str[str_len] = '\0';
  *arg = str;
  return true;
}

// callback function to encode a Strnum message
/**
 * @brief callback function to encode a Strnum message in a protobuf
 * message.
 *
 * @param stream The stream to write to.
 * @param field The field descriptor for the Strnum message.
 * @param arg The Strnum message to encode.
 *
 * @return True if encoding was successful, false otherwise.
 */
bool pb_encode_strnum(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const Strnum *msg = (const Strnum *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_submessage(stream, Strnum_fields, msg);
}