#include "gtest/gtest.h"

extern "C" {
#include "../../src/stack_stack_charptr_tptr.h"
#include "../../src/stack_stack_charptr_tptr.h"  // testing header guards
#include "string.h"
}

// Author: Marek Filip <xfilip46, Wecros>
// Date 27/11/2020
// Copied the behaviour of the tests from 'stack_charptr_tests.cpp'
// Tests for the stack of stacks containing pointer to char (string).

// Firstly, typedef the horrible type name to something sensible.
typedef stack_stack_charptr_tptr_t stack_of_stacks;
// And define a macro so we don't have to write the atrocious name every time.
#define STACK(F) stack_stack_charptr_tptr_ ## F

int stack_cmp(stack_charptr_t *a, stack_charptr_t *b) {
    if (a->top == b->top) {
        int top = a->top;
        while (top >= 0) {
            if (strcmp(a->array[top], b->array[top]) == 0) {
                return 0;
            }
            top--;
        }
    }
    return -1;
}

class stack_of_stacks_general : public::testing::Test {
    protected:
        stack_of_stacks *stack;
        stack_charptr_t *charptr_stack1;
        stack_charptr_t *charptr_stack2;
        stack_charptr_t *charptr_stack3;

        void SetUp() override {
            stack = STACK(init)();
            charptr_stack1 = stack_charptr_init();
            charptr_stack2 = stack_charptr_init();
            charptr_stack3 = stack_charptr_init();

            stack_charptr_push(charptr_stack1, "foo");
            stack_charptr_push(charptr_stack1, "bar");
            stack_charptr_push(charptr_stack2, "xyz");
            stack_charptr_push(charptr_stack2, "zyx");
            stack_charptr_push(charptr_stack3, "hello");
            stack_charptr_push(charptr_stack3, "world");
            stack_charptr_push(charptr_stack3, "!");
        }

        void TearDown() override {
            STACK(free)(stack);
            stack_charptr_free(charptr_stack1);
            stack_charptr_free(charptr_stack2);
            stack_charptr_free(charptr_stack3);
        }
};

TEST_F(stack_of_stacks_general, push_some_stacks) {
    // current charptr stack
    stack_charptr_t *curr;

    STACK(push)(stack, charptr_stack1);
    curr = STACK(pop)(stack);
    ASSERT_EQ(curr->array[1], "bar");
    ASSERT_EQ(curr->array[0], "foo");

    STACK(push)(stack, charptr_stack2);
    curr = STACK(pop)(stack);
    ASSERT_EQ(curr->array[1], "zyx");
    ASSERT_EQ(curr->array[0], "xyz");

    STACK(push)(stack, charptr_stack3);
    curr = STACK(pop)(stack);
    ASSERT_EQ(curr->array[2], "!");
    ASSERT_EQ(curr->array[1], "world");
    ASSERT_EQ(curr->array[0], "hello");

    STACK(push)(stack, charptr_stack3);
    // test is stack already present?
    ASSERT_EQ(STACK(ispresent)(stack, charptr_stack3, stack_cmp), true);
}

class stack_of_stacks_overflow : public::testing::Test {
    protected:
        stack_of_stacks *stack;
        stack_charptr_t *charptr_stack1;
        stack_charptr_t *charptr_stack2;
        stack_charptr_t *charptr_stack3;
        stack_charptr_t *charptr_stack4;
        stack_charptr_t *charptr_stack5;
        stack_charptr_t *charptr_stack6;
        stack_charptr_t *charptr_stack7;
        stack_charptr_t *charptr_stack8;
        stack_charptr_t *charptr_stack9;
        stack_charptr_t *charptr_stack10;
        stack_charptr_t *charptr_stack11;

        void SetUp() override {
            stack = STACK(init)();
            charptr_stack1 = stack_charptr_init();
            charptr_stack2 = stack_charptr_init();
            charptr_stack3 = stack_charptr_init();
            charptr_stack4 = stack_charptr_init();
            charptr_stack5 = stack_charptr_init();
            charptr_stack6 = stack_charptr_init();
            charptr_stack7 = stack_charptr_init();
            charptr_stack8 = stack_charptr_init();
            charptr_stack9 = stack_charptr_init();
            charptr_stack10 = stack_charptr_init();
            charptr_stack11 = stack_charptr_init();

            stack_charptr_push(charptr_stack1, "1");
            stack_charptr_push(charptr_stack1, "2");
            stack_charptr_push(charptr_stack1, "3");
            stack_charptr_push(charptr_stack1, "4");
            stack_charptr_push(charptr_stack1, "5");
            stack_charptr_push(charptr_stack1, "6");
            stack_charptr_push(charptr_stack1, "7");
            stack_charptr_push(charptr_stack1, "8");
            stack_charptr_push(charptr_stack1, "9");
            stack_charptr_push(charptr_stack1, "10");

            stack_charptr_push(charptr_stack2, "2");
            stack_charptr_push(charptr_stack3, "3");
            stack_charptr_push(charptr_stack4, "4");
            stack_charptr_push(charptr_stack5, "5");
            stack_charptr_push(charptr_stack6, "6");
            stack_charptr_push(charptr_stack7, "7");
            stack_charptr_push(charptr_stack8, "8");
            stack_charptr_push(charptr_stack9, "9");
            stack_charptr_push(charptr_stack10, "10");
            stack_charptr_push(charptr_stack11, "Not Yet");
            stack_charptr_push(charptr_stack11, "I am 11");

            STACK(push)(stack, charptr_stack1);
            STACK(push)(stack, charptr_stack2);
            STACK(push)(stack, charptr_stack3);
            STACK(push)(stack, charptr_stack4);
            STACK(push)(stack, charptr_stack5);
            STACK(push)(stack, charptr_stack6);
            STACK(push)(stack, charptr_stack7);
            STACK(push)(stack, charptr_stack8);
            STACK(push)(stack, charptr_stack9);
            STACK(push)(stack, charptr_stack10);
        }

        void TearDown() override {
            STACK(free)(stack);
            stack_charptr_free(charptr_stack1);
        }
};

TEST_F(stack_of_stacks_overflow, test_overflows) {
    // Test the charptr first
    // make sure the capacity is correct
    ASSERT_EQ(charptr_stack1->size, 10);
    // push the 10th value
    stack_charptr_push(charptr_stack1, "test");
    ASSERT_EQ(charptr_stack1->size, 20);
    ASSERT_EQ(stack_charptr_peek(charptr_stack1), "test");

    // now comes the stack of stacks
    ASSERT_EQ(stack->size, 10);
    STACK(push)(stack, charptr_stack11);
    ASSERT_EQ(stack->size, 20);
    stack_charptr_t *curr = STACK(peek)(stack);
    ASSERT_EQ(stack_charptr_peek(charptr_stack11), "I am 11");
}
