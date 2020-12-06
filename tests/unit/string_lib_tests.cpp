#include "gtest/gtest.h"

extern "C" {
#include <string.h>
#include "../../src/str.h"
}

/**
 * Test function present in str.h
 */

TEST(string_tests, prepend) {
    char *foo = "foo";
    foo = prepend(foo, "i");
    ASSERT_EQ(strcmp(foo, "ifoo"), 0);

    foo = prepend(foo, "fidorka");
    ASSERT_EQ(strcmp(foo, "fidorkaifoo"), 0);

    free(foo);
}

TEST(string_tests, convert_double) {
    double val = 3.14;

    char *foo = convert_double_to_string(val);
    ASSERT_EQ(strcmp(foo, "3.14"), 0);

    val = 8.9;
    foo = convert_double_to_hexa(val);
    ASSERT_EQ(strcmp(foo, "0x1.1cccccccccccdp+3"), 0);

    free(foo);
}

TEST(string_tests, convert_int) {
    int val = 420;

    char *foo = convert_int_to_string(val);
    ASSERT_EQ(strcmp(foo, "420"), 0);

    free(foo);
}
