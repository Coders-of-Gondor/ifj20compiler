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

class scanner_scan_tokens : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        int line;

        void SetUp() override {
            global_init();
            FILE *f = fopen("./samples/string_manipulation.ifj20", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
        }
};

TEST_F(scanner_scan_tokens, get_first_token) {
    bool eol_encounter = false;
    int result = scanner_scan(s, &t, &eol_encounter, &line);
    int token_type = t.type;
    ASSERT_EQ(result, 0);
    ASSERT_EQ(token_type, PACKAGE);
}

TEST_F(scanner_scan_tokens, get_more_tokens) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < 10; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (eol_encounter == true)
            break;
        if (i == 2 || i == 1)
            ASSERT_EQ(token_type, IDENT);
        else if (i == 2)
            ASSERT_EQ(token_type, FUNC);
        else if (i == 8)
            ASSERT_EQ(token_type, DEFINE);
        else if (i == 9)
            ASSERT_EQ(token_type, STRING_LIT);
    }
}

class scanner_scan_tokens_commentary : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        int line;

        void SetUp() override {
            global_init();
            FILE *f = fopen("./samples/eols.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
        }
};

TEST_F(scanner_scan_tokens_commentary, divide_or_comment) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < 10; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 0)                 // 1st token is 'PACKAGE'
            ASSERT_EQ(token_type, PACKAGE);
        else if (i == 1)            // 2nd token should be 'main' (identifier)
            ASSERT_EQ(token_type, IDENT);
        else if (i == 2)            // 3rd token should be 'FUNC'
            ASSERT_EQ(token_type, FUNC);
        else if (i == 3)            // 4th token should be 'add' (ident)
            ASSERT_EQ(token_type, IDENT);
        else if (i == 4)        // 5th token is '('
            ASSERT_EQ(token_type, LPAREN);
        else if (i == 5)        // 6th token is 'i', so should be IDENT
            ASSERT_EQ(token_type, IDENT);
        else if (i == 6)
            ASSERT_EQ(token_type, INT);
    }
}
