#include "gtest/gtest.h"

extern "C" {
#include "../../src/parser.h"
}

/* TEST the additional STACK functions START */
/** Functions to test:
 * int stack_top_nonterminal(stack_token_t_t *stack. token_t *value);
 * void stack_shift(stack_token_t_t *stack);
 * void stack_reduce(stack_token_t_t *stack, int nonterminal);
 *
 * Genera stack functions are already covered in the basic tests
 * so there's no need to cover the init, push etc. here.
 */
token_t general_token = { .type = INVALID };

class stack_empty : public::testing::Test {
    protected:
        stack_token_t_t *stack;

        void SetUp() override {
            stack = stack_token_t_init();
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
        }
};

TEST_F(stack_empty, empty_operations) {
    // test the return value
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), false); 
}

class stack_end_of_input : public::testing::Test {
    protected:
        stack_token_t_t *stack;

        void SetUp() override {
            stack = stack_token_t_init();
            // add the end of input onto the stack
            stack_token_t_push(stack, prec_create_EOI_symbol());
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
        }
};

TEST_F(stack_end_of_input, all_parser_stack_funcs) {
    // shift IDENT onto the stack
    token_t input = { .type = IDENT };
    stack_shift(stack, prec_create_EOI_symbol());
    stack_token_t_push(stack, input);
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[2].type, IDENT);

    token_t ident_token = { .type = IDENT };
    // test stack_token_t_at()
    token_t zero = stack_token_t_at(stack, 0);
    token_t one = stack_token_t_at(stack, 1);
    token_t two = stack_token_t_at(stack, 2);
    ASSERT_EQ(zero.type, END_OF_INPUT);
    ASSERT_EQ(one.type, SHF);
    ASSERT_EQ(two.type, IDENT);
    // test stack_token_t_find()
    ASSERT_EQ(stack_token_t_find(stack, prec_create_EOI_symbol(), cmp_token_t), 0);
    ASSERT_EQ(stack_token_t_find(stack, prec_create_shift_symbol(), cmp_token_t), 1);
    ASSERT_EQ(stack_token_t_find(stack, ident_token, cmp_token_t), 2);
    ASSERT_EQ(stack_token_t_find(stack, prec_create_expr_symbol(), cmp_token_t), -1);
    // test stack_top_nonterminal()
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);

    // reduce the IDENT to E
    stack_int_t *_ = stack_int_init();
    ASSERT_EQ(stack_reduce(stack, _), true);
    stack_int_free(_);
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    // test stack_token_t_at()
    zero = stack_token_t_at(stack, 0);
    one = stack_token_t_at(stack, 1);
    ASSERT_EQ(zero.type, END_OF_INPUT);
    ASSERT_EQ(one.type, EXPR_SYMBOL);
    // test stack_token_t_find()
    ASSERT_EQ(stack_token_t_find(stack, prec_create_EOI_symbol(), cmp_token_t), 0);
    ASSERT_EQ(stack_token_t_find(stack, prec_create_expr_symbol(), cmp_token_t), 1);
    ASSERT_EQ(stack_token_t_find(stack, ident_token, cmp_token_t), -1);
    // test stack_top_nonterminal()
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
}

class stack_i_plus_i_mul_i : public ::testing::Test {
    protected:
        stack_token_t_t *stack;
        int index = 0;
        token_type input_file[6] = {IDENT, ADD, IDENT, MUL, IDENT, END_OF_INPUT};
        token_t input = { .type = input_file[0] };
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_token_t_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_token_t_push(stack, prec_create_EOI_symbol());
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = { .type = input_file[index] };
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
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(input.type, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : +i*i$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : +i*i$
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<E+
    // Input   : i*i$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<E+<i
    // Input   : *i$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, IDENT);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<E+E
    // Input   : *i$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*
    // Input   : i$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6].type, MUL);
    ASSERT_EQ(stack->array[5].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*<i
    // Input   : $
    ASSERT_EQ(stack->top, 8);
    ASSERT_EQ(stack->array[8].type, IDENT);
    ASSERT_EQ(stack->array[7].type, SHF);
    ASSERT_EQ(stack->array[6].type, MUL);
    ASSERT_EQ(stack->array[5].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<E+<E*E
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6].type, MUL);
    ASSERT_EQ(stack->array[5].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<E+E
    // Input   : $
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, ADD);
    ASSERT_EQ(stack->array[2].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure
}

class stack_function : public ::testing::Test {
    protected:
        stack_token_t_t *stack;
        int index = 0;
        token_type input_file[9] = {IDENT, LPAREN, IDENT, COMMA, IDENT, ADD, FLOAT64_LIT, RPAREN, END_OF_INPUT};
        token_t input = { .type = input_file[0] };
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_token_t_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_token_t_push(stack, prec_create_EOI_symbol());
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = { .type = input_file[index] };
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
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(input.type, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : (i,i+l)$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<i(
    // Input   : i,i+l)$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<i(<i
    // Input   : ,i+l)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, IDENT);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E
    // Input   : ,i+l)$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,
    // Input   : i+l)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<i
    // Input   : +l)$
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7].type, IDENT);
    ASSERT_EQ(stack->array[6].type, SHF);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : +l)$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+
    // Input   : l)$
    ASSERT_EQ(stack->top, 8);
    ASSERT_EQ(stack->array[8].type, ADD);
    ASSERT_EQ(stack->array[7].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6].type, SHF);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[6]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+<l
    // Input   : )$
    ASSERT_EQ(stack->top, 10);
    ASSERT_EQ(stack->array[10].type, FLOAT64_LIT);
    ASSERT_EQ(stack->array[9].type, SHF);
    ASSERT_EQ(stack->array[8].type, ADD);
    ASSERT_EQ(stack->array[7].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6].type, SHF);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<E+E
    // Input   : )$
    ASSERT_EQ(stack->top, 9);
    ASSERT_EQ(stack->array[9].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[8].type, ADD);
    ASSERT_EQ(stack->array[7].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[6].type, SHF);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[7]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E)
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7].type, RPAREN);
    ASSERT_EQ(stack->array[6].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[8]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[8]); // just to be sure
}

class prec_func_in_params : public ::testing::Test {
    protected:
        stack_token_t_t *stack;
        int index = 0;
        token_type input_file[6] = {LPAREN, IDENT, LPAREN, RPAREN, RPAREN, END_OF_INPUT};
        token_t input = { .type = input_file[0] };
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_token_t_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_token_t_push(stack, prec_create_EOI_symbol());
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = { .type = input_file[index] };
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
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(input.type, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<(
    // Input   : i())$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<(<i
    // Input   : ())$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, IDENT);
    ASSERT_EQ(stack->array[3].type, SHF);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<(<i(
    // Input   : ))$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, LPAREN);
    ASSERT_EQ(stack->array[4].type, IDENT);
    ASSERT_EQ(stack->array[3].type, SHF);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<(<i()
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6].type, RPAREN);
    ASSERT_EQ(stack->array[5].type, LPAREN);
    ASSERT_EQ(stack->array[4].type, IDENT);
    ASSERT_EQ(stack->array[3].type, SHF);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<(E
    // Input   : )$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<(E)
    // Input   : $
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, RPAREN);
    ASSERT_EQ(stack->array[3].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[2].type, LPAREN);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure
}

class prec_func_with_2_params : public ::testing::Test {
    protected:
        stack_token_t_t *stack;
        int index = 0;
        token_type input_file[7] = {IDENT, LPAREN, IDENT, COMMA, IDENT, RPAREN, END_OF_INPUT};
        token_t input = { .type = input_file[0] };
        bool stop = false;
        stack_int_t *func_stack;

        void SetUp() override {
            stack = stack_token_t_init();
            func_stack = stack_int_init();
            // add the end of input onto the stack
            stack_token_t_push(stack, prec_create_EOI_symbol());
        }
        
        void TearDown() override {
            stack_token_t_free(stack);
            stack_int_free(func_stack);
        }

        void GetNextInput() {
            // get the next input from the "input file"
            index++;
            input = { .type = input_file[index] };
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
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(input.type, input_file[0]); // just to be sure

    Analyze();
    // Pushdown: $<i
    // Input   : (i,i)$
    ASSERT_EQ(stack->top, 2);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[1]); // just to be sure

    Analyze();
    // Pushdown: $<i(
    // Input   : i,i)$
    ASSERT_EQ(stack->top, 3);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[2]); // just to be sure

    Analyze();
    // Pushdown: $<i(<i
    // Input   : ,i)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, IDENT);
    ASSERT_EQ(stack->array[4].type, SHF);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E
    // Input   : ,i)$
    ASSERT_EQ(stack->top, 4);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[3]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,
    // Input   : i)$
    ASSERT_EQ(stack->top, 5);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[4]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,<i
    // Input   : )$
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7].type, IDENT);
    ASSERT_EQ(stack->array[6].type, SHF);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E
    // Input   : )$
    ASSERT_EQ(stack->top, 6);
    ASSERT_EQ(stack->array[6].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[5]); // just to be sure

    Analyze();
    // Pushdown: $<i(E,E)
    // Input   : $
    ASSERT_EQ(stack->top, 7);
    ASSERT_EQ(stack->array[7].type, RPAREN);
    ASSERT_EQ(stack->array[6].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[5].type, COMMA);
    ASSERT_EQ(stack->array[4].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[3].type, LPAREN);
    ASSERT_EQ(stack->array[2].type, IDENT);
    ASSERT_EQ(stack->array[1].type, SHF);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(input.type, input_file[6]); // just to be sure

    Analyze();
    // Pushdown: $E
    // Input   : $
    ASSERT_EQ(stack->top, 1);
    ASSERT_EQ(stack->array[1].type, EXPR_SYMBOL);
    ASSERT_EQ(stack->array[0].type, END_OF_INPUT);
    stack_top_nonterminal(stack, &general_token);
    ASSERT_EQ(stack_top_nonterminal(stack, &general_token), true);
    ASSERT_EQ(general_token.type, END_OF_INPUT);
    ASSERT_EQ(input.type, input_file[6]); // just to be sure
}

/* TEST the additional STACK functions END */

