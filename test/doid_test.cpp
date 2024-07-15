#include "main_test.h"

// Helper functions
void assert_DoId_equal(const DoId *a, const DoId *b) {
  TEST_ASSERT_EQUAL_INT(a->id, b->id);
  TEST_ASSERT_FLOAT_WITHIN(
      0.0001, a->value,
      b->value); // Allow a small margin for floating-point comparison
}

// Test cases
void test_serialize_DoId() {
  DoId message = DoId_init_default;
  message.id = 42;
  message.value = 3.14;

  pb_byte_t buffer[128];
  size_t bytes_written;
  bool success =
      serialize_DoId(&message, buffer, sizeof(buffer), &bytes_written);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_GREATER_THAN(0, bytes_written);

  // Check that the serialized data is not just zeros
  TEST_ASSERT_NOT_EQUAL(0, buffer[0]);

  printf("test_serialize_DoId passed.\n");
}

void test_deserialize_DoId() {
  DoId original_message = DoId_init_default;
  original_message.id = 42;
  original_message.value = 3.14;

  pb_byte_t buffer[128];
  size_t bytes_written;
  serialize_DoId(&original_message, buffer, sizeof(buffer), &bytes_written);

  DoId new_message = DoId_init_default;
  bool success = deserialize_DoId(buffer, bytes_written, &new_message);

  TEST_ASSERT_TRUE(success);
  assert_DoId_equal(&original_message, &new_message);
  printf("test_deserialize_DoId passed.\n");
}

void test_serialize_deserialize_DoId() {
  DoId message = DoId_init_default;
  message.id = 123;
  message.value = 4.56;

  pb_byte_t buffer[128];
  size_t bytes_written;

  bool success =
      serialize_DoId(&message, buffer, sizeof(buffer), &bytes_written);
  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_GREATER_THAN(0, bytes_written);

  DoId new_message = DoId_init_default;
  success = deserialize_DoId(buffer, bytes_written, &new_message);

  TEST_ASSERT_TRUE(success);
  assert_DoId_equal(&message, &new_message);
  printf("test_serialize_deserialize_DoId passed.\n");
}

void test_serialize_DoId_invalid_buffer() {
  DoId message = DoId_init_default;
  message.id = 1;
  message.value = 1.0;

  pb_byte_t buffer[5]; // Buffer too small
  size_t bytes_written;
  bool success =
      serialize_DoId(&message, buffer, sizeof(buffer), &bytes_written);

  TEST_ASSERT_FALSE(
      success); // Serialization should fail due to insufficient buffer size
  TEST_ASSERT_EQUAL_UINT(
      0, bytes_written); // Ensure no bytes were written to the buffer
  printf("test_serialize_DoId_invalid_buffer passed.\n");
}

void test_deserialize_DoId_invalid_buffer() {
  DoId message = DoId_init_default;

  pb_byte_t invalid_buffer[128];
  memset(invalid_buffer, 0, sizeof(invalid_buffer)); // Invalid serialized data
  size_t buffer_size = sizeof(invalid_buffer);
  bool success = deserialize_DoId(invalid_buffer, buffer_size, &message);

  TEST_ASSERT_FALSE(success); // Deserialization should fail
  printf("test_deserialize_DoId_invalid_buffer passed.\n");
}
