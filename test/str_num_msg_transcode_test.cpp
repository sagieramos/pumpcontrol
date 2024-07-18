#include "main_test.h"
#include <stdio.h>
#include <str_num_msg_transcode.h>

// Test creating a strnum message
void test_create_strunum() {
  const char *test_str = "Hello, World!";
  float test_num = 42.0f;
  uint32_t test_key = 12345;
  strnum msg;

  create_strunum(test_str, test_num, test_key, &msg);

  TEST_ASSERT_EQUAL_STRING(test_str, (const char *)msg.str.arg);
  TEST_ASSERT_EQUAL_FLOAT(test_num, msg.num);
  TEST_ASSERT_EQUAL_UINT32(test_key, msg.key);
}

// Test serializing and deserializing a strnum message
void test_serialize_deserialize_strnum() {
  const char *test_str = "Hello, World!";
  float test_num = 42.0f;
  uint32_t test_key = 12345;
  strnum msg;
  create_strunum(test_str, test_num, test_key, &msg);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 10;

  printf("size of buffer before strnum serialization: %ld\n", buffer_size);

  TEST_ASSERT_TRUE(serialize_strnum(msg, buffer, &buffer_size, type_id));
  for (int i = 0; i < sizeof(buffer); i++) {
    printf("%x ", buffer[i]);
  }
  printf("\n");
  printf("size of buffer after strnum serialization: %ld\n", buffer_size);

  strnum msg_out;
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
  strnum msg;
  create_strunum(test_str, test_num, test_key, &msg);

  strnumlst msg_lst;
  create_strnumlst(&msg, &msg_lst);

  uint8_t buffer[128];
  size_t buffer_size = sizeof(buffer);
  uint8_t type_id = 2;

  TEST_ASSERT_TRUE(
      serialize_strnumlst(&msg_lst, buffer, &buffer_size, type_id));

  strnumlst msg_lst_out;
  TEST_ASSERT_TRUE(deserialize_strnumlst(msg_lst_out, buffer, buffer_size));

  strnum *msg_out = (strnum *)msg_lst_out.str_nums.arg;

  TEST_ASSERT_EQUAL_UINT32(msg.key, msg_out->key);
  TEST_ASSERT_EQUAL_FLOAT(msg.num, msg_out->num);
  TEST_ASSERT_EQUAL_STRING((const char *)msg.str.arg,
                           (const char *)msg_out->str.arg);
}