#include "gtest/gtest.h"

extern "C" {
#include <string.h>
#include "../../src/three-address-code.h"
}

TEST(TAC_tests, converts) {
    item result;
    result = TAC_get_int(420);
    ASSERT_EQ(strcmp(result, "i420"), 0);

    result = TAC_get_float(3.14);
    ASSERT_EQ(strcmp(result, "f3.14"), 0);

    string foo;
    strInit(&foo);
    strAddChar(&foo, 'p');
    strAddChar(&foo, 'e');
    strAddChar(&foo, 's');
    result = TAC_get_string(foo);
    ASSERT_EQ(strcmp(result, "spes"), 0);
    strFree(&foo);

    result = TAC_get_identifier((char *) "foo");
    ASSERT_EQ(strcmp(result, "dfoo"), 0);

    result = TAC_create_function_argument(1);
    ASSERT_EQ(strcmp(result, "&arg1"), 0);
    result = TAC_create_function_argument(523);
    ASSERT_EQ(strcmp(result, "&arg523"), 0);
}

TEST(TAC_tests, convert_term) {
    item result;
    token_t token;
    string foo;

    token.type = INT_LIT;
    token.attribute.int_val = 120;
    result = TAC_convert_term(token);
    ASSERT_EQ(strcmp(result, "i120"), 0);

    token.type = FLOAT64_LIT;
    token.attribute.float_val = 2.73490;
    result = TAC_convert_term(token);
    ASSERT_EQ(strcmp(result, "f2.7349"), 0);

    strInit(&foo);
    strAddChar(&foo, 'o');
    strAddChar(&foo, 'k');
    strAddChar(&foo, 'o');
    strAddChar(&foo, 'k');
    strAddChar(&foo, 'o');
    strAddChar(&foo, 'k');
    strAddChar(&foo, '\\');
    strAddChar(&foo, 't');
    strAddChar(&foo, '\\');
    strAddChar(&foo, 'n');
    strAddChar(&foo, '\\');
    strAddChar(&foo, '\\');
    token.type = STRING_LIT;
    token.attribute.str_val = foo;
    result = TAC_convert_term(token);
    ASSERT_EQ(strcmp(result, "sokokok\\t\\n\\\\"), 0);
    strFree(&foo);

    token.type = IDENT;
    token.attribute.sym_key = (char *)"best_VarEver";
    result = TAC_convert_term(token);
    ASSERT_EQ(strcmp(result, "dbest_VarEver"), 0);

    token.type = RETURN;
    result = TAC_convert_term(token);
    ASSERT_EQ(result, NULL);
}
