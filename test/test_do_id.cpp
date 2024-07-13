#define TESTING
#include <domsgid.h>
#include <unity.h>

void test_do_id() {
  DoId obj = DoId_init_zero;
  obj.id = 123;
  obj.value = 3.14;

  uint8_t buffer[128];
  size_t bytes_written = 0;
  int status = serialize_do_id(buffer, sizeof(buffer), obj);
  TEST_ASSERT_EQUAL_INT(8, status);

  DoId obj2 = DoId_init_zero;
  status = deserialize_do_id(buffer, sizeof(buffer), obj2);
  TEST_ASSERT_EQUAL_INT(8, status);
  TEST_ASSERT_EQUAL_INT(obj.id, obj2.id);
  TEST_ASSERT_EQUAL_FLOAT(obj.value, obj2.value);
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_do_id);
  UNITY_END();
}
