#include "str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>

// Callback function to encode a string
bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

bool pb_encode_strnum(pb_ostream_t *stream, const pb_field_t *field,
                      void *const *arg) {
  const strnum *msg = (const strnum *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  if (!pb_encode_submessage(stream, strnum_fields, msg)) {
    return false;
  }
  return true;
}

void create_strunum(const char *str, float num, uint32_t key, strnum *msg) {
  *msg = strnum_init_zero;
  msg->key = key;
  msg->num = num;
  if (str != NULL) {
    msg->str.funcs.encode = &pb_encode_string;
    msg->str.arg = (void *)str;
  }
}

void create_strnumlst(const strnum *strum, strnumlst *msg) {
  *msg = strnumlst_init_zero;
  msg->str_nums.funcs.encode = &pb_encode_strnum;
  msg->str_nums.arg = (void *)strum;
}

bool serialize_strnum(strnum &msg, uint8_t *buffer, size_t *buffer_size,
                      uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, strnum_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_strnum(strnum &msg, const uint8_t *buffer,
                        size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, strnum_fields, &msg);
}

bool serialize_strnumlst(const strnumlst *msg, uint8_t *buffer,
                         size_t *buffer_size, uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, strnumlst_fields, msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_strnumlst(strnumlst &msg, const uint8_t *buffer,
                           size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, strnumlst_fields, &msg);
}
