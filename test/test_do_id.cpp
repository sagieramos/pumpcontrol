#include <domsgid.h>
#include <unity.h>

void test_serialize_single_do_id() {
  uint8_t buffer[256];
  DoId obj;
  obj.id = 1;
  obj.value = 100;

  int result = serialize_do_id(buffer, sizeof(buffer), obj);
  TEST_ASSERT_TRUE(result > 0);
}

void test_deserialize_single_do_id() {
  uint8_t buffer[256];
  DoId original_obj;
  original_obj.id = 1;
  original_obj.value = 100;

  int serialize_result = serialize_do_id(buffer, sizeof(buffer), original_obj);
  TEST_ASSERT_TRUE(serialize_result > 0);

  DoId deserialized_obj;
  int deserialize_result =
      deserialize_do_id(buffer, serialize_result, deserialized_obj);

  TEST_ASSERT_EQUAL(original_obj.id, deserialized_obj.id);
  TEST_ASSERT_EQUAL(original_obj.value, deserialized_obj.value);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_serialize_single_do_id);
  RUN_TEST(test_deserialize_single_do_id);
  UNITY_END();
  return 0;
}
