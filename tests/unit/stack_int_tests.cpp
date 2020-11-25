#include "gtest/gtest.h"

extern "C" {
#define TYPE int
#include "../../src/stack.h"
#undef TYPE
}

// Author: Marek Filip <xfilip46, Wecros>
// Date 25/11/2020
// Copied the behaviour of the tests from 'scanner_tests.cpp'

class stack_int_empty : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
        }
        
        void TearDown() override {
            stack_int_free(&stack);
        }
};

TEST_F(stack_int_empty, init) {
    ASSERT_NE(stack, NULL);
    ASSERT_EQ(stack->size, STACK_START_SIZE);
    ASSERT_EQ(stack->top, -1);
    ASSERT_NE(stack->array, NULL);
}

TEST_F(stack_int_empty, push_some_values) {
    stack_int_push(stack, 5);
    stack_int_push(stack, 10);
    stack_int_push(stack, 15);

    ASSERT_EQ(stack->array[0], 5);
    ASSERT_EQ(stack->array[1], 10);
    ASSERT_EQ(stack->array[2], 15);
}

class stack_int_general : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
            stack_int_push(stack, 5);
            stack_int_push(stack, 10);
            stack_int_push(stack, 15);
        }
        
        void TearDown() override {
            stack_int_free(&stack);
        }
};

TEST_F(stack_int_general, push_some_values) {
    stack_int_push(stack, 5);
    stack_int_push(stack, 10);
    stack_int_push(stack, 15);

    ASSERT_EQ(stack->array[0], 5);
    ASSERT_EQ(stack->array[1], 10);
    ASSERT_EQ(stack->array[2], 15);
}

TEST_F(stack_int_general, peek_top) {
    int val = stack_int_peek(stack);
    ASSERT_EQ(val, 15);
}

TEST_F(stack_int_general, pop_some_values) {
    ASSERT_EQ(15, stack_int_pop(stack));
    ASSERT_EQ(10, stack_int_pop(stack));
    ASSERT_EQ(5, stack_int_pop(stack));
}

// class to be destroyed
class stack_int_destroy : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
            stack_int_push(stack, 'x');
            stack_int_push(stack, 'y');
            stack_int_push(stack, 'z');
        }
        
        void TearDown() override {
            // do nothing
        }
};

// test the successful deallocation
TEST_F(stack_int_destroy, free_the_stack) {
    stack_int_free(&stack);
    ASSERT_EQ(stack, NULL);
}

class stack_int_full : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
            stack_int_push(stack, 1);
            stack_int_push(stack, 2);
            stack_int_push(stack, 3);
            stack_int_push(stack, 4);
            stack_int_push(stack, 5);
            stack_int_push(stack, 6);
            stack_int_push(stack, 7);
            stack_int_push(stack, 8);
            stack_int_push(stack, 9);
            stack_int_push(stack, 10);
        }
        
        void TearDown() override {
            stack_int_free(&stack);
        }
};

TEST_F(stack_int_full, push_some_values) {
    stack_int_push(stack, 5);
    stack_int_push(stack, 10);
    stack_int_push(stack, 15);

    ASSERT_EQ(stack->array[10], 5);
    ASSERT_EQ(stack->array[11], 10);
    ASSERT_EQ(stack->array[12], 15);
}

TEST_F(stack_int_full, pop_some_values) {
    ASSERT_EQ(10, stack_int_pop(stack));
    ASSERT_EQ(9, stack_int_pop(stack));
    ASSERT_EQ(8, stack_int_pop(stack));
}

// complex test to make sure resize works correctly
TEST_F(stack_int_full, complex_test) {
    // push one value to make the stack bigger  
    stack_int_push(stack, 11);
    ASSERT_EQ(stack->array[10], 11);

    // peek and pop
    ASSERT_EQ(11, stack_int_peek(stack));
    ASSERT_EQ(11, stack_int_pop(stack));

    // push again
    stack_int_push(stack, 11);
    ASSERT_EQ(stack->array[10], 11);
}
