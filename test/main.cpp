#include "main_test.h"

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_DoId);
  RUN_TEST(test_deserialize_DoId);
  RUN_TEST(test_serialize_deserialize_DoId);
  RUN_TEST(test_serialize_DoId_invalid_buffer);
  RUN_TEST(test_deserialize_DoId_invalid_buffer);

  RUN_TEST(test_create_strunum);
  RUN_TEST(test_serialize_deserialize_strnum);
  // RUN_TEST(test_serialize_deserialize_strnumlst);

  return UNITY_END();
}
