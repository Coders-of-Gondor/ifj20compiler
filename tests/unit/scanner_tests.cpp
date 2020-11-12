#include "gtest/gtest.h"

extern "C" {
#include "../../src/global.h"
#include "../../src/scanner-private.h"
#include "../../src/str.h"
#include "../../src/scanner.h"
#include "../../src/token.h"
}

class scanner_basic_tests : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        
        void SetUp() override {
          s = scanner_new(NULL);
        }

        void TearDown() override {
          scanner_free(s);
        }
};

TEST_F(scanner_basic_tests, initialize) {
    ASSERT_NE(s, NULL);
    ASSERT_EQ(s->file, NULL);
    ASSERT_EQ(s->position, 0);
}

TEST_F(scanner_basic_tests, call_scan_with_no_file_set) {
    bool eol_encountered = false;
    int line = 0;
    int ret = scanner_scan(s, &t, &eol_encountered, &line);
    ASSERT_EQ(ret, 99);
}

class scanner_component_testing : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;

        void SetUp() override {
            s = scanner_new(NULL);
        }

        void TearDown() override {
            scanner_free(s);
        }
};

TEST_F(scanner_component_testing, scan_string_lit) {
    char literal[] = "test_string\"";
    int num_of_chars = 12;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_string_lit(s);
        if (i == 11)
            ASSERT_EQ(ret, 2);
        else
            ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_decimal1) {
    char literal[] = "123456";
    int num_of_chars = 6;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_decimal2) {
    char literal[] = "123e21";
    int num_of_chars = 6;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_float1) {
    char literal[] = "123.321";
    int num_of_chars = 7;
    
    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_float2) {
    char literal[] = "123.321e21";
    int num_of_chars = 10;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_float3) {
    char literal[] = "123.321E21";
    int num_of_chars = 10;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

class scanner_scanning_valid_sourcefile : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;

        void SetUp() override {
            global_init();
            FILE *f = fopen("./samples/string_manipulation.ifj20", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
        }
};

TEST_F(scanner_scanning_valid_sourcefile, single_scan_call) {
    bool eol_encountered = false;
    int line = 0;
    int ret = scanner_scan(s, &t, &eol_encountered, &line);
    ASSERT_EQ(ret, 0);
}

TEST_F(scanner_scanning_valid_sourcefile, scan_until_eof) {
    int ret;
    int line = 0;
    bool eol_encountered = false;
    do {
        ret = scanner_scan(s, &t, &eol_encountered, &line);
        if (ret != EOF)
            ASSERT_EQ(ret, 0);
    } while (ret != EOF);
}
