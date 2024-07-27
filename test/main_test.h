#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include "./str_num_msg_transcode.h"
#include <unity.h>

void test_create_strunum();
void test_serialize_deserialize_strnum();

void test_serialize_str();
void test_deserialize_str();
void test_serialize_deserialize_auth();
void test_serialize_deserialize_numlist();

#endif // MAIN_TEST_H