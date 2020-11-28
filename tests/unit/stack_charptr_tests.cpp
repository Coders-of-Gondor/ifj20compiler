#include "gtest/gtest.h"

extern "C" {
#include "../../src/stack_charptr.h"
#include "../../src/stack_charptr.h"  // testing header guards
#include "string.h"
}

// Author: Marek Filip <xfilip46, Wecros>
// Date 27/11/2020
// Copied the behaviour of the tests from 'stack_int_tests.cpp'

class stack_charptr_general : public::testing::Test {
    protected:
        stack_charptr_t *stack;

        void SetUp() override {
            stack = stack_charptr_init();
            stack_charptr_push(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+");
            stack_charptr_push(stack, "bar");
            stack_charptr_push(stack, "foo");
        }
        
        void TearDown() override {
            stack_charptr_free(stack);
        }
};

TEST_F(stack_charptr_general, push_some_values) {
    stack_charptr_push(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+");
    stack_charptr_push(stack, "bar");
    stack_charptr_push(stack, "foo");

    ASSERT_EQ(stack->array[0], "ifj20codejetennejlepsikamo,.=-=+asdf234__+");
    ASSERT_EQ(stack->array[1], "bar");
    ASSERT_EQ(stack->array[2], "foo");
}

TEST_F(stack_charptr_general, test_number_of_occurences) {
    ASSERT_EQ(stack_charptr_howmany(stack, "bar", strcmp), 1);
    ASSERT_EQ(stack_charptr_howmany(stack, "foo", strcmp), 1);
    ASSERT_EQ(stack_charptr_howmany(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+", strcmp), 1);
    ASSERT_EQ(stack_charptr_howmany(stack, "notpresent", strcmp), 0);
    ASSERT_EQ(stack_charptr_howmany(stack, "askldfuj234/\n", strcmp), 0);

    stack_charptr_push(stack, "bar");
    ASSERT_EQ(stack_charptr_howmany(stack, "bar", strcmp), 2);

    stack_charptr_push(stack, "\n\t\n");
    ASSERT_EQ(stack_charptr_howmany(stack, "\n\t\n", strcmp), 1);
    stack_charptr_push(stack, "\n\t\n");
    ASSERT_EQ(stack_charptr_howmany(stack, "\n\t\n", strcmp), 2);

    ASSERT_EQ(stack_charptr_howmany(stack, "\n\t", strcmp), 0);
    stack_charptr_push(stack, "\n\t");
    ASSERT_EQ(stack_charptr_howmany(stack, "\n\t", strcmp), 1);
        
    stack_charptr_push(stack, "\n\t\n");
    ASSERT_EQ(stack_charptr_howmany(stack, "\n\t\n", strcmp), 3);
}


TEST_F(stack_charptr_general, peek_top) {
    char *val = stack_charptr_peek(stack);
    ASSERT_EQ(val, "foo");
}

TEST_F(stack_charptr_general, pop_some_values) {
    ASSERT_EQ(stack_charptr_ispresent(stack, "foo", strcmp), true);
    ASSERT_EQ(stack_charptr_ispresent(stack, "bar", strcmp), true);
    ASSERT_EQ(stack_charptr_ispresent(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+", strcmp), true);

    ASSERT_EQ("foo", stack_charptr_pop(stack));
    ASSERT_EQ(stack_charptr_ispresent(stack, "foo", strcmp), false);
    ASSERT_EQ(stack_charptr_ispresent(stack, "bar", strcmp), true);
    ASSERT_EQ(stack_charptr_ispresent(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+", strcmp), true);

    ASSERT_EQ("bar", stack_charptr_pop(stack));
    ASSERT_EQ(stack_charptr_ispresent(stack, "foo", strcmp), false);
    ASSERT_EQ(stack_charptr_ispresent(stack, "bar", strcmp), false);
    ASSERT_EQ(stack_charptr_ispresent(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+", strcmp), true);

    ASSERT_EQ("ifj20codejetennejlepsikamo,.=-=+asdf234__+", stack_charptr_pop(stack));
    ASSERT_EQ(stack_charptr_ispresent(stack, "foo", strcmp), false);
    ASSERT_EQ(stack_charptr_ispresent(stack, "bar", strcmp), false);
    ASSERT_EQ(stack_charptr_ispresent(stack, "ifj20codejetennejlepsikamo,.=-=+asdf234__+", strcmp), false);
}
