#include "./transcode_msg1.h"
#include <pb_decode.h>
#include <pb_encode.h>

bool serialize_msg1(const Msg1 &msg1, uint8_t *buffer, size_t *buffer_size,
                    uint8_t type_id, msg1_codec_callback_t cb) {
  buffer[0] = type_id;

  pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 1, *buffer_size - 1);
  bool status = pb_encode(&ostream, Msg1_fields, &msg1);

  *buffer_size = ostream.bytes_written + 1;

  if (cb && status) {
    cb(static_cast<void *>(buffer), *buffer_size);
  }

  return status;
}

bool deserialize_msg1(const uint8_t *buffer, size_t buffer_size, Msg1 &msg1,
                      msg1_codec_callback_t cb) {
  pb_istream_t istream = pb_istream_from_buffer(buffer + 1, buffer_size - 1);
  bool status = pb_decode(&istream, Msg1_fields, &msg1);

  if (cb && status) {
    cb((void *)buffer, buffer_size);
  }

  return status;
}