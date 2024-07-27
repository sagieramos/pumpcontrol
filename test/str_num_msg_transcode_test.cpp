#include "main_test.h"
#include "str_num_msg_transcode.h"
#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdio.h>

// Test serializing a Str message
void test_serialize_str() {
  Str msg = Str_init_zero;
  const char *test_str = "Hello, Imuwahen!";

  create_str(test_str, 2, msg);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 2;

  TEST_ASSERT_TRUE(serialize_str(msg, buffer, &buffer_size, type_id, NULL));

  for (int i = 0; i < sizeof(buffer); i++) {
    printf("%x ", buffer[i]);
  }

  printf("Size of buffer after Str serialization..........: %ld\n",
         buffer_size);
  for (size_t i = 0; i < buffer_size; i++) {
    printf("%x ", buffer[i]);
  }
  printf("\n");

  TEST_ASSERT_EQUAL_STRING(test_str, msg.value.arg);

  Str msg_out = Str_init_zero;
  TEST_ASSERT_TRUE(deserialize_str(msg_out, buffer, buffer_size, NULL));

  TEST_ASSERT_EQUAL_STRING(test_str, msg_out.value.arg);

  free_str(msg_out);
}

// Test deserializing a Str message
void test_deserialize_str() {
  const char *test_str = "Hello, World!";
  Str msg = Str_init_zero;
  // create_str(test_str, 100, msg);
  create_str("Hello, World!", 100, msg);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 10;

  TEST_ASSERT_TRUE(serialize_str(msg, buffer, &buffer_size, type_id, NULL));

  Str msg_out;
  TEST_ASSERT_TRUE(deserialize_str(msg_out, buffer, buffer_size, NULL));

  TEST_ASSERT_EQUAL_STRING(test_str, msg_out.value.arg);

  free_str(msg_out);
}

// Test creating a strnum message
void test_create_strunum() {
  const char *test_str = "Hello, World!";
  float test_num = 42.0f;
  uint32_t test_key = 12345;
  Strnum msg;

  create_strnum(test_str, test_num, test_key, msg);

  TEST_ASSERT_EQUAL_STRING(test_str, (const char *)msg.str.arg);
  TEST_ASSERT_EQUAL_FLOAT(test_num, msg.num);
  TEST_ASSERT_EQUAL_UINT32(test_key, msg.key);
}

// Test serializing and deserializing a strnum message
void test_serialize_deserialize_strnum() {
  const char *test_str = "Hello, World!";
  float test_num = 42.0f;
  uint32_t test_key = 12345;
  Strnum msg;
  create_strnum(test_str, test_num, test_key, msg);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 3;

  printf("size of buffer before strnum serialization: %ld\n", buffer_size);

  TEST_ASSERT_TRUE(serialize_strnum(msg, buffer, &buffer_size, type_id));
  for (int i = 0; i < sizeof(buffer); i++) {
    printf("%x ", buffer[i]);
  }
  printf("\n");
  printf("size of buffer after strnum serialization: %ld\n", buffer_size);

  Strnum msg_out;
  TEST_ASSERT_TRUE(deserialize_strnum(msg_out, buffer, buffer_size));

  TEST_ASSERT_EQUAL_UINT32(msg.key, msg_out.key);
  TEST_ASSERT_EQUAL_FLOAT(msg.num, msg_out.num);
  printf("string.........................%s\n", (const char *)msg_out.str.arg);
  TEST_ASSERT_EQUAL_STRING((const char *)msg.str.arg,
                           (const char *)msg_out.str.arg);

  free_strnum(msg_out);

  // Test msg_out.str is NULL
  TEST_ASSERT_NULL(msg_out.str.arg);
}

// Test serializing and deserialiasing a Auth message
void test_serialize_deserialize_auth() {
  Auth msg = Auth_init_zero;
  const char *test_id = "imuwahen";
  const char *test_pass = "password";

  create_auth(test_id, test_pass, msg);

  uint8_t buffer[555];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 4;

  TEST_ASSERT_TRUE(serialize_auth(msg, buffer, &buffer_size, type_id, NULL));

  Auth msg_out = Auth_init_zero;
  TEST_ASSERT_TRUE(deserialize_auth(msg_out, buffer, buffer_size, NULL));

  TEST_ASSERT_EQUAL_STRING(test_id, msg_out.id.arg);
  TEST_ASSERT_EQUAL_STRING(test_pass, msg_out.pass.arg);

  free_auth(msg_out);
}

bool encode_repeated_field(pb_ostream_t *stream, const pb_field_iter_t *field,
                           void *const *arg) {
  uint64_t *values = (uint64_t *)*arg; // Access the array of values
  size_t count =
      field->array_size; // Use field->array_size for the number of elements

  for (size_t i = 0; i < count; ++i) {
    if (!pb_encode_varint(stream, values[i])) {
      return false; // Handle encoding error
    }
  }
  return true;
}

bool decode_repeated_field(pb_istream_t *stream, const pb_field_iter_t *field,
                           void **arg) {
  uint64_t *values =
      (uint64_t *)*arg; // Access the array to store decoded values
  size_t index = 0;

  while (stream->bytes_left) {
    if (index >= field->array_size) {
      return false; // Handle overflow if more values are read than expected
    }

    if (!pb_decode_varint(stream, &values[index])) {
      return false; // Handle decoding error
    }

    ++index;
  }
  return true;
}

// Test serializing and deserializing a Numlist message
void test_serialize_deserialize_numlist() {

  float test_values[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  Numlist msg = Numlist_init_zero;

  uint8_t buffer[128];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  bool status = pb_encode(&stream, Numlist_fields, &msg);
  if (status) {
    printf("Encoding successful\n");
  } else {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
  }

  TEST_ASSERT_TRUE(status);
}
