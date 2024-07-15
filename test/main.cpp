#include "main_test.h"

int main(void) {
  UNITY_BEGIN(); // Initialize Unity test framework

  RUN_TEST(test_serialize_DoId);
  RUN_TEST(test_deserialize_DoId);
  RUN_TEST(test_serialize_deserialize_DoId);
  RUN_TEST(test_serialize_DoId_invalid_buffer);
  RUN_TEST(test_deserialize_DoId_invalid_buffer);

  return UNITY_END(); // Finalize Unity test framework and return the result
}
