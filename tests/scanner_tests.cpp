#include "gtest/gtest.h"

extern "C" {
#include "../src/scanner-private.h"
}

class basic_scan_tests : public ::testing::Test {
    protected:
        scanner_t *s = scanner_new(NULL);
};

TEST_F(basic_scan_tests, initialize) {
    ASSERT_NE(s, NULL);
    ASSERT_EQ(s->file, NULL);
    ASSERT_EQ(s->position, 0);
}
