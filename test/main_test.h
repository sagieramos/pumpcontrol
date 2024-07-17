#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include <unity.h>

void test_serialize_DoId();
void test_deserialize_DoId();
void test_serialize_deserialize_DoId();
void test_serialize_DoId_invalid_buffer();
void test_deserialize_DoId_invalid_buffer();

void test_create_strunum();
void test_serialize_deserialize_strnum();
void test_serialize_deserialize_strnumlst();

#endif // MAIN_TEST_H