#include "main_test.h"
#include "str_num_msg_transcode.h"
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
  create_str(test_str, 100, msg);

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

// Test serializing and deserializing a strnumlst message
void test_serialize_deserialize_strnumlst() {
  const char *test_str = "Hello, World!";
  float test_num = 42.0f;
  uint32_t test_key = 12345;
  Strnum msg;
  create_strnum(test_str, test_num, test_key, msg);

  Strnumlist msg_lst;
  create_strnumlst(&msg, msg_lst);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 2;

  TEST_ASSERT_TRUE(
      serialize_strnumlst(&msg_lst, buffer, &buffer_size, type_id));

  Strnumlist msg_lst_out;
  TEST_ASSERT_TRUE(deserialize_strnumlst(msg_lst_out, buffer, buffer_size));

  Strnum *msg_out = (Strnum *)msg_lst_out.str_nums.arg;

  TEST_ASSERT_EQUAL_UINT32(msg.key, msg_out->key);
  TEST_ASSERT_EQUAL_FLOAT(msg.num, msg_out->num);
  TEST_ASSERT_EQUAL_STRING((const char *)msg.str.arg,
                           (const char *)msg_out->str.arg);
}