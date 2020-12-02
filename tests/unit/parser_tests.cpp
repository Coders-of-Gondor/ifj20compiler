#include "gtest/gtest.h"

extern "C" {
#include "../../src/parser.h"
}

/* TEST the additional STACK functions START */
/** Functions to test:
 * int stack_top(stack_int_t *stack);
 * void stack_shift(stack_int_t *stack);
 * void stack_reduce(stack_int_t *stack, int nonterminal);
 *
 * Genera stack functions are already covered in the basic tests
 * so there's no need to cover the init, push etc. here.
 */

class stack_empty : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
        }
        
        void TearDown() override {
            stack_int_free(stack);
        }
};

TEST_F(stack_empty, empty_operations) {
    // test the return value
    ASSERT_EQ(stack_top(stack), -1); 
}

class stack_end_of_input : public::testing::Test {
    protected:
        stack_int_t *stack;

        void SetUp() override {
            stack = stack_int_init();
            // add the end of input onto the stack
            stack_int_push(stack, END_OF_INPUT);
        }
        
        void TearDown() override {
            stack_int_free(stack);
        }
};

TEST_F(stack_end_of_input, all_parser_stack_funcs) {
    // shift IDENT onto the stack
    token_type input = IDENT;
    stack_shift(stack, END_OF_INPUT);
    stack_int_push(stack, input);
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[2], IDENT);

    // test stack_at()
    ASSERT_EQ(stack_at(stack, 0), END_OF_INPUT);
    ASSERT_EQ(stack_at(stack, 1), SHF);
    ASSERT_EQ(stack_at(stack, 2), IDENT);
    // test stack_find()
    ASSERT_EQ(stack_find(stack, END_OF_INPUT), 0);
    ASSERT_EQ(stack_find(stack, SHF), 1);
    ASSERT_EQ(stack_find(stack, IDENT), 2);
    ASSERT_EQ(stack_find(stack, EXPR_SYMBOL), -1);
    // test stack_top()
    ASSERT_EQ(stack_top(stack), IDENT);

    // reduce the IDENT to E
    stack_int_t *_ = stack_int_init();
    ASSERT_EQ(stack_reduce(stack, _), true);
    stack_int_free(_);
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    // test stack_at()
    ASSERT_EQ(stack_at(stack, 0), END_OF_INPUT);
    ASSERT_EQ(stack_at(stack, 1), EXPR_SYMBOL);
    // test stack_find()
    ASSERT_EQ(stack_find(stack, END_OF_INPUT), 0);
    ASSERT_EQ(stack_find(stack, EXPR_SYMBOL), 1);
    ASSERT_EQ(stack_find(stack, IDENT), -1);
    // test stack_top()
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
}

class stack_i_plus_i_mul_i : public ::testing::Test {
    protected:
        stack_int_t *stack;
        int index = 0;
        token_type input_file[6] = {IDENT, ADD, IDENT, MUL, IDENT, END_OF_INPUT};
        token_type input = input_file[0];
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_int_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_int_push(stack, END_OF_INPUT);
        }
        
        void TearDown() override {
            stack_int_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = input_file[index];
        }

        void Analyze() {
            // analyze the input
            if (prec_handle_symbol(stack, input, &stop, func_stack)) {
                GetNextInput();
            }
        }
};

// Test the succesful analysis with correct stack structure
// of the expression "i + i * i"
TEST_F(stack_i_plus_i_mul_i, reduce) {
    // Pushdown: $
    // Input   : i+i*i$
    ASSERT_EQ(stack->top, 0);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(input, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : +i*i$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : +i*i$
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
    ASSERT_EQ(input, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<E+
    // Input   : i*i$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), ADD);
    ASSERT_EQ(input, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<E+<i
    // Input   : *i$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], IDENT);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<E+E
    // Input   : *i$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), ADD);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*
    // Input   : i$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6], MUL);
    ASSERT_EQ(stack->array[5], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), MUL);
    ASSERT_EQ(input, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*<i
    // Input   : $
    ASSERT_EQ(stack->top, 8);
    ASSERT_EQ(stack->array[8], IDENT);
    ASSERT_EQ(stack->array[7], SHF);
    ASSERT_EQ(stack->array[6], MUL);
    ASSERT_EQ(stack->array[5], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*E
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6], MUL);
    ASSERT_EQ(stack->array[5], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), MUL);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<E+E
    // Input   : $
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], ADD);
    ASSERT_EQ(stack->array[2], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), ADD);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
    ASSERT_EQ(input, input_file[5]); // just to be sure
}

class stack_function : public ::testing::Test {
    protected:
        stack_int_t *stack;
        int index = 0;
        token_type input_file[9] = {IDENT, LPAREN, IDENT, COMMA, IDENT, ADD, FLOAT64_LIT, RPAREN, END_OF_INPUT};
        token_type input = input_file[0];
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_int_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_int_push(stack, END_OF_INPUT);
        }
        
        void TearDown() override {
            stack_int_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = input_file[index];
        }

        void Analyze() {
            // analyze the input
            if (prec_handle_symbol(stack, input, &stop, func_stack)) {
                GetNextInput();
            }
        }
};

TEST_F(stack_function, parse_the_function) {
    // Pushdown: $
    // Input   : i(i,i+l)$
    ASSERT_EQ(stack->top, 0);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(input, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : (i,i+l)$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<i(
    // Input   : i,i+l)$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<i(<i
    // Input   : ,i+l)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], IDENT);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E
    // Input   : ,i+l)$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,
    // Input   : i+l)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), COMMA);
    ASSERT_EQ(input, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<i
    // Input   : +l)$
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7], IDENT);
    ASSERT_EQ(stack->array[6], SHF);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : +l)$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), COMMA);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+
    // Input   : l)$
    ASSERT_EQ(stack->top, 8);
    ASSERT_EQ(stack->array[8], ADD);
    ASSERT_EQ(stack->array[7], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6], SHF);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), ADD);
    ASSERT_EQ(input, input_file[6]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+<l
    // Input   : )$
    ASSERT_EQ(stack->top, 10);
    ASSERT_EQ(stack->array[10], FLOAT64_LIT);
    ASSERT_EQ(stack->array[9], SHF);
    ASSERT_EQ(stack->array[8], ADD);
    ASSERT_EQ(stack->array[7], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6], SHF);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), FLOAT64_LIT);
    ASSERT_EQ(input, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+E
    // Input   : )$
    ASSERT_EQ(stack->top, 9);
    ASSERT_EQ(stack->array[9], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[8], ADD);
    ASSERT_EQ(stack->array[7], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6], SHF);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), ADD);
    ASSERT_EQ(input, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), COMMA);
    ASSERT_EQ(input, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E)
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7], RPAREN);
    ASSERT_EQ(stack->array[6], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), RPAREN);
    ASSERT_EQ(input, input_file[8]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
    ASSERT_EQ(input, input_file[8]); // just to be sure
}

class prec_func_in_params : public ::testing::Test {
    protected:
        stack_int_t *stack;
        int index = 0;
        token_type input_file[6] = {LPAREN, IDENT, LPAREN, RPAREN, RPAREN, END_OF_INPUT};
        token_type input = input_file[0];
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_int_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_int_push(stack, END_OF_INPUT);
        }
        
        void TearDown() override {
            stack_int_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = input_file[index];
        }

        void Analyze() {
            // analyze the input
            if (prec_handle_symbol(stack, input, &stop, func_stack)) {
                GetNextInput();
            }
        }
};

TEST_F(prec_func_in_params, func_in_params) {
    // Pushdown: $
    // Input   : (i())$
    ASSERT_EQ(stack->top, 0);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(input, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<(
    // Input   : i())$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<(<i
    // Input   : ())$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], IDENT);
    ASSERT_EQ(stack->array[3], SHF);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<(<i(
    // Input   : ))$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], LPAREN);
    ASSERT_EQ(stack->array[4], IDENT);
    ASSERT_EQ(stack->array[3], SHF);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<(<i()
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6], RPAREN);
    ASSERT_EQ(stack->array[5], LPAREN);
    ASSERT_EQ(stack->array[4], IDENT);
    ASSERT_EQ(stack->array[3], SHF);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), RPAREN);
    ASSERT_EQ(input, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<(E
    // Input   : )$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<(E)
    // Input   : $
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], RPAREN);
    ASSERT_EQ(stack->array[3], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[2], LPAREN);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), RPAREN);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
    ASSERT_EQ(input, input_file[5]); // just to be sure
}

class prec_func_with_2_params : public ::testing::Test {
    protected:
        stack_int_t *stack;
        int index = 0;
        token_type input_file[7] = {IDENT, LPAREN, IDENT, COMMA, IDENT, RPAREN, END_OF_INPUT};
        token_type input = input_file[0];
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_int_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_int_push(stack, END_OF_INPUT);
        }
        
        void TearDown() override {
            stack_int_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = input_file[index];
        }

        void Analyze() {
            // analyze the input
            if (prec_handle_symbol(stack, input, &stop, func_stack)) {
                GetNextInput();
            }
        }
};

TEST_F(prec_func_with_2_params, parse) {
    // Pushdown: $
    // Input   : i(i,i)$
    ASSERT_EQ(stack->top, 0);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(input, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : (i,i)$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<i(
    // Input   : i,i)$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<i(<i
    // Input   : ,i)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], IDENT);
    ASSERT_EQ(stack->array[4], SHF);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E
    // Input   : ,i)$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), LPAREN);
    ASSERT_EQ(input, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,
    // Input   : i)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), COMMA);
    ASSERT_EQ(input, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<i
    // Input   : )$
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7], IDENT);
    ASSERT_EQ(stack->array[6], SHF);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), IDENT);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), COMMA);
    ASSERT_EQ(input, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E)
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7], RPAREN);
    ASSERT_EQ(stack->array[6], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5], COMMA);
    ASSERT_EQ(stack->array[4], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3], LPAREN);
    ASSERT_EQ(stack->array[2], IDENT);
    ASSERT_EQ(stack->array[1], SHF);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), RPAREN);
    ASSERT_EQ(input, input_file[6]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1], EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0], END_OF_INPUT);
    ASSERT_EQ(stack_top(stack), END_OF_INPUT);
    ASSERT_EQ(input, input_file[6]); // just to be sure
}

/* TEST the additional STACK functions END */

