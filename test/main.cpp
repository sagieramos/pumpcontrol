#include "main_test.h"

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_create_strunum);
  RUN_TEST(test_serialize_deserialize_strnum);
  RUN_TEST(test_serialize_str);
  RUN_TEST(test_deserialize_str);
  RUN_TEST(test_serialize_deserialize_auth);
  RUN_TEST(test_serialize_deserialize_numlist);

  return UNITY_END();
}
