#include "./str_num_msg_transcode.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdlib.h>

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
  const Strnum *msg = (const Strnum *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  if (!pb_encode_submessage(stream, Strnum_fields, msg)) {
    return false;
  }
  return true;
}

bool pb_decode_string(pb_istream_t *stream, const pb_field_t *field,
                      void **arg) {
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

void create_num(float number, uint32_t key, Num *msg) {
  *msg = Num_init_zero;
  msg->key = key;
  msg->value = number;
}

void create_str(const char *str, uint32_t key, Strnum *msg) {
  *msg = Strnum_init_zero;
  msg->key = key;
  if (str != NULL) {
    msg->str.funcs.encode = &pb_encode_string;
    msg->str.arg = (void *)str;
  }
}

void create_strnum(const char *str, float num, uint32_t key, Strnum *msg) {
  *msg = Strnum_init_zero;
  msg->key = key;
  msg->num = num;
  if (str != NULL) {
    msg->str.funcs.encode = &pb_encode_string;
    msg->str.arg = (void *)str;
  }
}

void create_strnumlst(const Strnum *strum, Strnumlist *msg) {
  *msg = Strnumlist_init_zero;
  msg->str_nums.funcs.encode = &pb_encode_strnum;
  msg->str_nums.arg = (void *)strum;
}

bool serialize_num(const Num &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Num_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_num(Num &msg, const uint8_t *buffer, size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, Num_fields, &msg);
}

bool serialize_str(const Str &msg, uint8_t *buffer, size_t *buffer_size,
                   uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Str_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_str(Str &msg, const uint8_t *buffer, size_t buffer_size) {
  msg = Str_init_zero;
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, Str_fields, &msg);
}

void free_str(Str &msg) {
  if (msg.value.arg) {
    free(msg.value.arg);
    msg.value.arg = NULL;
  }
}

bool serialize_strnum(Strnum &msg, uint8_t *buffer, size_t *buffer_size,
                      uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Strnum_fields, &msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_strnum(Strnum &msg, const uint8_t *buffer,
                        size_t buffer_size) {
  msg = Strnum_init_zero;
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  msg.str.funcs.decode = &pb_decode_string;

  return (pb_decode(&stream, Strnum_fields, &msg));
}

void free_strnum(Strnum &msg) {
  if (msg.str.arg) {
    free(msg.str.arg);
    msg.str.arg = NULL;
  }
}

bool serialize_strnumlst(const Strnumlist *msg, uint8_t *buffer,
                         size_t *buffer_size, uint8_t type_id) {
  buffer[0] = type_id;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&stream, Strnumlist_fields, msg);
  if (status) {
    *buffer_size = stream.bytes_written + 1;
  }
  return status;
}

bool deserialize_strnumlst(Strnumlist &msg, const uint8_t *buffer,
                           size_t buffer_size) {
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  return pb_decode(&stream, Strnumlist_fields, &msg);
}

/*

#define MAX_STR_LEN 128

bool pb_decode_fixed_string(pb_istream_t *stream, const pb_field_t *field, void
**arg) { char *str = (char *)*arg; size_t str_len = stream->bytes_left; if
(str_len > MAX_STR_LEN) { return false;
    }

    if (!pb_read(stream, (pb_byte_t *)str, str_len)) {
        return false;
    }
    str[str_len] = '\0';

    return true;
}


bool deserialize_strnum(strnum &msg, const uint8_t *buffer, size_t buffer_size)
{ msg = strnum_init_zero; char str[MAX_STR_LEN + 1]; msg.str.funcs.decode =
&pb_decode_fixed_string; msg.str.arg = str; pb_istream_t stream =
pb_istream_from_buffer(buffer + 1, buffer_size - 1); bool status =
pb_decode(&stream, strnum_fields, &msg); if (status) { msg.str.arg =
strdup(str); // Duplicate the string to allocate on the heap
    }

    return status;
} */
