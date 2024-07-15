#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include "domsgid.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_serialize_DoId();
void test_deserialize_DoId();
void test_serialize_deserialize_DoId();
void test_serialize_DoId_invalid_buffer();
void test_deserialize_DoId_invalid_buffer();

#endif // MAIN_TEST_H