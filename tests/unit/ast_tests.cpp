#include "gtest/gtest.h"

extern "C" {
#include "../../src/ast.h"
}

TEST(ast_parameter, basic_operation) {
    func_parameter_t *p = func_parameter_new();
    ASSERT_NE(p, NULL);
    func_parameter_free(p);
}

TEST(ast_return, basic_operation) {
    func_return_t *r = func_return_new();
    ASSERT_NE(r, NULL);
    func_return_free(r);
}

TEST(ast_function, basic_operation) {
    func_t f;
    func_init(&f);

    ASSERT_EQ(f.first_parameter, NULL);
    ASSERT_EQ(f.first_return, NULL);
    ASSERT_EQ(f.num_of_parameters, 0);
    ASSERT_EQ(f.num_of_returns, 0);

    func_free(&f);
}
