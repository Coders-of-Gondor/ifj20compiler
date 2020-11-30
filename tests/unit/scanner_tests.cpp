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
        int line;
        
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
    int ret = scanner_scan(s, &t, 0, &line);
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

TEST_F(scanner_component_testing, scan_num_lit_decimal_exponent_zero) {
    char literal[] = "123e000000";
    int num_of_chars = 10;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_float_exponent_zero) {
    char literal[] = "12.3e000000";
    int num_of_chars = 11;

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

TEST_F(scanner_component_testing, scan_num_lit_float1_5) {
    char literal[] = "0.0";
    int num_of_chars = 3;
    
    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, scan_num_lit_float2) {
    char literal[] = "123.321e20";
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

TEST_F(scanner_component_testing, scan_num_lit_float4) {
    char literal[] = "20";
    int num_of_chars = 2;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        int ret = scan_num_lit(s, &t);
        ASSERT_EQ(ret, 0);
    }
}

TEST_F(scanner_component_testing, innit_scan_single_char_tokens) {
    char literals[][2] = { ",", ";", "(", ")", "{", "}", };
    int num_of_literals = sizeof(literals) / sizeof(literals[0]);

    for (int i = 0; i < num_of_literals; i++) {
        s->character = literals[i][0];
        s->state = INIT;

        int ret = innit_scan(s, &t);
        ASSERT_EQ(ret, 0);
        ASSERT_EQ(s->state, STOP);
    }
}

TEST_F(scanner_component_testing, innit_scan_double_char_tokens) {
    char literals[][3] = { "==", "<=", ">=", ":=", "!="};
    int num_of_literals = sizeof(literals) / sizeof(literals[0]);

    for (int i = 0; i < num_of_literals; i++) {
        s->file = tmpfile();
        fprintf(s->file, "%s", literals[i]);
        rewind(s->file);

        s->character = fgetc(s->file);
        s->state = INIT;

        int ret = innit_scan(s, &t);
        ASSERT_EQ(ret, 0);
        ASSERT_EQ(s->state, STOP);

        fclose(s->file);
    }
}

TEST_F(scanner_component_testing, innit_scan_number) {
    // This is tested per-character, not as a whole string
    char literal[] = "0123456789";
    int num_of_chars = 10;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        s->state = INIT;

        int ret = innit_scan(s, &t);
        ASSERT_EQ(ret, 0);
        if (i == 0)
            ASSERT_EQ(s->state, f15);
        else
            ASSERT_EQ(s->state, f14);
    }
}

TEST_F(scanner_component_testing, innit_scan_identifier) {
    // This is tested per-character, not as a whole string
    char literal[] = "_abcd";
    int num_of_chars = 5;

    for (int i = 0; i < num_of_chars; i++) {
        s->character = literal[i];
        s->state = INIT;

        int ret = innit_scan(s, &t);
        ASSERT_EQ(ret, 0);
        ASSERT_EQ(s->state, f10);
    }
}

TEST_F(scanner_component_testing, innit_scan_string_literal) {
    s->character = '"';
    s->state = INIT;

    int ret = innit_scan(s, &t);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(s->state, q3);
}

class scanner_scanning_valid_sourcefile : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;

        void SetUp() override {
            global_init();
            f = fopen("./samples/example3.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scanning_valid_sourcefile, single_scan_call) {
    bool eol_encountered = false;
    int ret = scanner_scan(s, &t, &eol_encountered, &line);
    ASSERT_EQ(ret, 0);
}

TEST_F(scanner_scanning_valid_sourcefile, scan_until_eof) {
    int ret;
    bool eol_encountered = false;
    do {
        ret = scanner_scan(s, &t, &eol_encountered, &line);
        if (ret != EOF)
            ASSERT_EQ(ret, 0);
    } while (ret != EOF);
}

class scanner_scanning_string_hex: public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;

        void SetUp() override {
            global_init();
            f = fopen("./samples/string_escape_hex.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scanning_string_hex, scan_hex_lit) {
    int ret;
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
        FILE *f;
        int line;

        void SetUp() override {
            global_init();
            f = fopen("./samples/example3.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
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
    for (int i = 0; i < 10; i++) { // loop through the first 10 tokens
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
        FILE *f;
        int line;

        void SetUp() override {
            global_init();
            f = fopen("./samples/eols.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scan_tokens_commentary, divide_or_comment) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < 10; i++) {  // loop through the first 10 tokens
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

class scanner_scan_tokens_zero_end : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;

        void SetUp() override {
            global_init();
            f = fopen("./samples/scope.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scan_tokens_zero_end, number_ends_with_zero) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < 11; i++) {  // loop through the first 11 tokens
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
            ASSERT_EQ(token_type, RPAREN);
        else if (i == 8)
            ASSERT_EQ(token_type, DEFINE);
        else if (i == 9)
            ASSERT_EQ(token_type, INT_LIT);
        else if (i == 10)
            ASSERT_EQ(token_type, IF);
    }
}

class scanner_scan_token_unary : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;
        int number_of_tokens_in_file = 36;

        void SetUp() override {
            global_init();
            f = fopen("./samples/unary.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scan_token_unary, ADD_ASSIGN) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 11)       // 12th is '+=' (ADD_ASSIGN)
            ASSERT_EQ(token_type, ADD_ASSIGN);
    }
}

TEST_F(scanner_scan_token_unary, SUB_ASSIGN) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 17)        // 18th is '-=' (SUB_ASSIGN)
            ASSERT_EQ(token_type, SUB_ASSIGN);
    }
}

TEST_F(scanner_scan_token_unary, MUL_ASSIGN) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 20)        // 21st is '*=' (MUL_ASSIGN)
            ASSERT_EQ(token_type, MUL_ASSIGN);
    }
}

TEST_F(scanner_scan_token_unary, DIV_ASSIGN) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 23)        // 24th is '/=' (DIV_ASSIGN)
            ASSERT_EQ(token_type, DIV_ASSIGN);
    }
}

TEST_F(scanner_scan_token_unary, STR_LIT) {
    bool eol_encounter = false;
    int result = 1;
    string str;
    strInit(&str);
    for (int k = 0; k < 4; k++) {
        if (k == 0)
            strAddChar(&str, 't');
        else if (k == 1)
            strAddChar(&str, 'e');
        else if (k == 2)
            strAddChar(&str, 'x');
        else if (k == 3)
            strAddChar(&str, 't');
    }
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 35) {        // 36th is "test" (STRING_LIT)
            ASSERT_EQ(token_type, STRING_LIT);
            ASSERT_EQ(strCmpString(&t.attribute.str_val, &str), 0);   // strcmpString is basically strcmp and that returns 0 if they're equal
        }   
    }
    strFree(&str);
}

TEST_F(scanner_scan_token_unary, INT_LIT) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 12) {        // 13th is 'INT_LIT'
            ASSERT_EQ(t.attribute.int_val, 1);
            ASSERT_EQ(token_type, INT_LIT);
        }
    }
}

TEST_F(scanner_scan_token_unary, FLOAT_LIT) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 15) {      // 16th is 'FLOAT_LIT'
            ASSERT_EQ(t.attribute.float_val, 1.5);
            ASSERT_EQ(token_type, FLOAT64_LIT);
        }
    }
}

class scanner_scan_divide : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;
        int number_of_tokens_in_file = 15;

        void SetUp() override {
            global_init();
            f = fopen("./samples/divide.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scan_divide, scan_divide) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 10) 
            ASSERT_EQ(token_type, IDENT);
        else if (i == 13)        // 13th is '/' (DIV)
            ASSERT_EQ(token_type, DIV);
        else if (i == 11)
            ASSERT_EQ(token_type, DEFINE);
        else if (i == 12)
            ASSERT_EQ(token_type, IDENT);
        else if (i == 14)
            ASSERT_EQ(token_type, IDENT);
    }
}

class scanner_scan_underscore : public ::testing::Test {
    protected:
        scanner_t *s;
        token_t t;
        FILE *f;
        int line;
        int number_of_tokens_in_file = 11;

        void SetUp() override {
            global_init();
            f = fopen("./samples/underscore.go", "r");
            s = scanner_new(f);
        }

        void TearDown() override {
            scanner_free(s);
            fclose(f);
        }
};

TEST_F(scanner_scan_underscore, scan_underscore) {
    bool eol_encounter = false;
    int result = 1;
    for (int i = 0; i < number_of_tokens_in_file; i++) {
        result = scanner_scan(s, &t, &eol_encounter, &line);
        ASSERT_EQ(result, 0);
        int token_type = t.type;
        if (i == 7) 
            ASSERT_EQ(token_type, IDENT);
        else if (i == 8) 
            ASSERT_EQ(token_type, ASSIGN);
        else if (i == 9) 
            ASSERT_EQ(token_type, INT_LIT);
        
    }
}
