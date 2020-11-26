#include "gtest/gtest.h"

extern "C" {
// can include like this because cpp does not need extern definitions for some reason.
#define TYPE char
#include "../../src/stack.h"
#undef TYPE
}

// Author: Marek Filip <xfilip46, Wecros>
// Date 25/11/2020
// Copied the behaviour of the tests from 'scanner_tests.cpp'

class stack_char_empty : public::testing::Test {
    protected:
        stack_char_t *stack;

        void SetUp() override {
            stack = stack_char_init();
        }
        
        void TearDown() override {
            stack_char_free(stack);
        }
};

TEST_F(stack_char_empty, init) {
    ASSERT_NE(stack, NULL);
    ASSERT_EQ(stack->size, STACK_START_SIZE);
    ASSERT_EQ(stack->top, -1);
    ASSERT_NE(stack->array, NULL);
}

TEST_F(stack_char_empty, push_some_values) {
    // test the correct return code if push was succesful
    ASSERT_EQ(stack_char_push(stack, 'x'), true);
    ASSERT_EQ(stack_char_push(stack, 'y'), true);
    ASSERT_EQ(stack_char_push(stack, 'z'), true);

    ASSERT_EQ(stack->array[0], 'x');
    ASSERT_EQ(stack->array[1], 'y');
    ASSERT_EQ(stack->array[2], 'z');
}

class stack_char_general : public::testing::Test {
    protected:
        stack_char_t *stack;

        void SetUp() override {
            stack = stack_char_init();
            stack_char_push(stack, 'x');
            stack_char_push(stack, 'y');
            stack_char_push(stack, 'z');
        }
        
        void TearDown() override {
            stack_char_free(stack);
        }
};

TEST_F(stack_char_general, push_some_values) {
    stack_char_push(stack, 'x');
    stack_char_push(stack, 'y');
    stack_char_push(stack, 'z');

    ASSERT_EQ(stack->array[0], 'x');
    ASSERT_EQ(stack->array[1], 'y');
    ASSERT_EQ(stack->array[2], 'z');
}

TEST_F(stack_char_general, peek_top) {
    int val = stack_char_peek(stack);
    ASSERT_EQ(val, 'z');
}

TEST_F(stack_char_general, pop_some_values) {
    ASSERT_EQ('z', stack_char_pop(stack));
    ASSERT_EQ('y', stack_char_pop(stack));
    ASSERT_EQ('x', stack_char_pop(stack));
}

class stack_char_full : public::testing::Test {
    protected:
        stack_char_t *stack;

        void SetUp() override {
            stack = stack_char_init();
            stack_char_push(stack, 'a');
            stack_char_push(stack, 'b');
            stack_char_push(stack, 'c');
            stack_char_push(stack, 'd');
            stack_char_push(stack, 'e');
            stack_char_push(stack, 'f');
            stack_char_push(stack, 'g');
            stack_char_push(stack, 'h');
            stack_char_push(stack, 'i');
            stack_char_push(stack, 'j');
        }
        
        void TearDown() override {
            stack_char_free(stack);
        }
};

TEST_F(stack_char_full, push_some_values) {
    stack_char_push(stack, 'x');
    stack_char_push(stack, 'y');
    stack_char_push(stack, 'z');

    ASSERT_EQ(stack->array[10], 'x');
    ASSERT_EQ(stack->array[11], 'y');
    ASSERT_EQ(stack->array[12], 'z');
}

TEST_F(stack_char_full, pop_some_values) {
    ASSERT_EQ('j', stack_char_pop(stack));
    ASSERT_EQ('i', stack_char_pop(stack));
    ASSERT_EQ('h', stack_char_pop(stack));
}

// complex test to make sure resize works correctly
TEST_F(stack_char_full, complex_test) {
    // push one value to make the stack bigger  
    stack_char_push(stack, '1');
    ASSERT_EQ(stack->array[10], '1');

    // peek and pop
    ASSERT_EQ('1', stack_char_peek(stack));
    ASSERT_EQ('1', stack_char_pop(stack));

    // push again
    stack_char_push(stack, 'Z');
    ASSERT_EQ(stack->array[10], 'Z');
}
