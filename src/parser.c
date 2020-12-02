/**
 * @file parser.c
 * @author Marek Filip <xfilip46>
 * @author Ondřej Míchal <xmicha80>
 * @brief Parser part of ifj20 compiler - the heart of the compiler.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "debug.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include "symtable.h"
#include "token.h"

scanner_t *scanner; /**< scanner local to the parser */
bool eol_encountered; /**< tracker to tell us if an eol was scanned */
int line = 1; /**< track the current line number */

token_t lookahead; /**< next token to look at */

bool eof_found = false; /**< track if the eof has been found, terminate the analysis correctly */
bool no_eol = false; /**< track if the eol could have been inserted before the token */
bool required_eol = false; /**< track if the eol should be required here */

symtable_t *st; /**< holds symbol stacks for every function */

symtable_t *st_internal_funcs; /** holds signatures of all internal functions */
symtable_t *st_called_funcs; /** holds signatures of all called functions */

// TODO: make eol rules

char *internal_func_names[] = {
    "inputs", "inputi", "inputf",
    "print",
    "int2float", "float2int",
    "len", "substr", "ord", "chr"
};

void parser_move() {
    debug_entry();

    eol_encountered = false;
    // move the lookahead
    int return_code = scanner_scan(scanner, &lookahead, &eol_encountered, &line);

    // TODO: make sure all return cases are handled here

    if (return_code == ERROR_LEXICAL) {
        parser_end(ERROR_LEXICAL);
    } else if (return_code == ERROR_INTERNAL) {
        parser_end(ERROR_INTERNAL);
    } else if (return_code == EOF) {
        eof_found = true;
    }

    if (lookahead.type == INVALID && return_code != EOF) {
        fprintf(stderr, "!!! INVALID TOKEN !!!\n");
        parser_end(ERROR_LEXICAL);
    }

    required_eol = false;
}

void parser_match(token_type t) {
    debug_entry();
    if (lookahead.type == t) {
        debug("Matched type: %s", token_get_type_string(t));
        debug_lit_value(lookahead);

        // EOL CHECKING
        debug("got eol? %d no_eol? %d required_eol? %d", eol_encountered, no_eol, required_eol);
        if (eol_encountered && no_eol) {
            fprintf(stderr, "Wrong EOL placement!\n");
            parser_end(ERROR_SYNTAX);
        } else if (!eol_encountered && required_eol) {
            fprintf(stderr, "EOL should be here!\n");
            parser_end(ERROR_SYNTAX);
        }

        parser_move();

    } else {
        debug("Expected type: %s", token_get_type_string(t));
        debug("Got type: %s", token_get_type_string(lookahead.type));
        debug_lit_value(lookahead);

        parser_end(ERROR_SYNTAX);
    }
}

void parser_match_ident(char *ident_name) {
    debug_entry();
    if (lookahead.type == IDENT && strcmp(lookahead.attribute.sym_key, ident_name) == 0) {
        debug("Matched type and string: %s, %s",
                token_get_type_string(IDENT), ident_name);
        parser_move();
    } else {
        debug("Expected type and ident name: %s, %s",
                token_get_type_string(IDENT), ident_name);
        debug("Got type and ident name: %s, %s",
                token_get_type_string(IDENT), lookahead.attribute.sym_key);
        parser_end(ERROR_SYNTAX);
    }
}

/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_start(scanner_t *scanner_main) {
    debug_entry();

    // Prepare scanner
    scanner = scanner_main;

    // Initialize symtable
    st = symtable_new();
    if (st == NULL)
        parser_end(ERROR_INTERNAL);

    st_internal_funcs = symtable_new();
    if (st_internal_funcs == NULL)
        parser_end(ERROR_INTERNAL);

    st_called_funcs = symtable_new();
    if (st_called_funcs == NULL)
        parser_end(ERROR_INTERNAL);

    const int NUM_OF_FUNCS = 10;
    for (int i = 0; i < NUM_OF_FUNCS; i++) {
        symtable_new_scope(st_internal_funcs, internal_func_names[i]);
    }

    // move the lookahead to the first lexeme
    parser_move();

    parser_prolog();
    parser_funcs();

    parser_end(SUCCESS);
}

void parser_end(int rc) {
    debug_entry();

    // Check if all called functions are defined
    symtable_set_first_scope(st_called_funcs);
    // FIXME: Symtable always starts with an unnamed scope. Until I come up
    // with a proper approach, move to the next scope.
    symtable_next_scope(st_called_funcs);
    do {
        char *func_id = symtable_get_scope_name(st_called_funcs);
        if (rc != SUCCESS) {
            break;
        }

        func_parameter_t *call_param = symtable_get_func_param(st_called_funcs);

        // Compare every called function identifier with every known function
        // internal functions
        if (symtable_set_current_scope(st_internal_funcs, func_id))
            continue;

        // user-defined functions
        if (symtable_set_current_scope(st, func_id)) {
            // When a match is found, compare the parameters
            func_parameter_t *def_param = symtable_get_func_param(st);

            // Check if parameter types are satisfied
            while (def_param != NULL && call_param != NULL) {
                if (def_param->type != call_param->type) {
                    fprintf(stderr, "Mismatched function parameters\n");
                    rc = ERROR_SEM_PROGRAM;
                    break;
                }

                def_param = def_param->next_param;
                call_param = call_param->next_param;
            }

            // Check if all parameters are satisfied
            if (def_param != NULL || call_param != NULL) {
                if (def_param->type != call_param->type) {
                    fprintf(stderr, "Mismatched function parameters\n");
                    rc = ERROR_SEM_PROGRAM;
                    break;
                }
            }

            continue;
        }

        fprintf(stderr, "Function %s is not defined\n", func_id);
        rc = ERROR_SEM_VAR;
    } while (symtable_next_scope(st_called_funcs));

    symtable_free(st);
    symtable_free(st_called_funcs);
    symtable_free(st_internal_funcs);

    if (rc == ERROR_LEXICAL)
        throw_lex_error(line);
    else if (rc == ERROR_SYNTAX)
        throw_syntax_error(lookahead.type, line);
    else if (rc >= ERROR_SEM_VAR && rc <= ERROR_ZERO)
        throw_semantics_error(rc, line);
    else if (rc == ERROR_INTERNAL)
        throw_internal_error();
}

void parser_prolog() {
    debug_entry();
    parser_match(PACKAGE);
    parser_match_ident("main");
}

void parser_funcs() {
    debug_entry();

    if (eof_found) {
        fprintf(stderr, "EOF encountered, stopping syntax analysis...\n");
        fprintf(stderr, "line: %d\n", line);
        return;
    }

    // if FUNC not found, apply eps-rule
    if (lookahead.type == FUNC) {
        // function decleration
        // New function -> new local scope of symbols
        parser_match(FUNC);

        char *func_id = lookahead.attribute.sym_key;
        no_eol = true;
        parser_match(IDENT);

        debug("New local scope - %s", func_id);
        if (!symtable_new_scope(st, func_id)) {
            fprintf(stderr, "Function with name %s already exists\n", func_id);
            parser_end(ERROR_SEM_VAR);
        }

        parser_params();
        symtable_push_stack(st);
        parser_r_params();
        parser_block();
        parser_funcs();
    }
    // apply eps-rule

    // if EOF not found, throw syntax error, program should not end here.
    if (!eof_found)
        parser_end(ERROR_SYNTAX);
}

void parser_stmts() {
    debug_entry();
    switch (lookahead.type) {
        case IDENT:
        case IF:
        case FOR:
        case RETURN:
            parser_stmt();
            parser_stmts();
            break; default: // apply eps-rule
            break;
    }
}

void parser_stmt() {
    debug_entry();
    char *ident = lookahead.attribute.sym_key;
    switch (lookahead.type) {
        case IDENT:
            // identifier found
            parser_match(IDENT);
            parser_id_first(ident);
            break;
        case IF:
            // if statement
            parser_stmt_if();
            break;
        case FOR:
            // for loop
            parser_stmt_for();
            break;
        case RETURN:
            // return statement
            parser_return();
            required_eol = true;
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_id_first(char *id) {
    debug_entry();

    switch (lookahead.type) {
        case DEFINE:
            // variable definition
            parser_vardef(id);
            required_eol = true;
            break;
        case ASSIGN:
        case COMMA:
            // variable assign
            parser_assign(id);
            required_eol = true;
            break;
        case LPAREN:
            // function call
            // Store identifier of the called function
            parser_func_call(id);
            required_eol = true;
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

/* ------------------------------------------------------------------------ */
/* GENERAL HELP RULES                                                       */
/* ------------------------------------------------------------------------ */

void parser_type() {
    debug_entry();
    switch (lookahead.type) {
        case INT:
            parser_match(INT);
            break;
        case FLOAT64:
            parser_match(FLOAT64);
            break;
        case STRING:
            parser_match(STRING);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_block() {
    debug_entry();
    parser_match(LBRACE);
    no_eol = false;
    required_eol = true;
    parser_stmts();
    parser_match(RBRACE);
}

/* ------------------------------------------------------------------------ */
/* STATEMENT RULES                                                          */
/* ------------------------------------------------------------------------ */

void parser_stmt_if() {
    debug_entry();

    parser_match(IF);

    symtable_push_stack(st);

    no_eol = true;
    parser_expr();
    parser_block();
    parser_match(ELSE);
    no_eol = true;
    parser_block();
    required_eol = true;

    symtable_push_stack(st);
}

void parser_stmt_for() {
    debug_entry();

    parser_match(FOR);

    symtable_push_stack(st);

    no_eol = true;
    parser_optdef();
    parser_match(SEMICOLON);
    parser_expr();
    parser_match(SEMICOLON);
    parser_optassign();
    parser_block();
    required_eol = true;

    symtable_push_stack(st);
}

/* ------------------------------------------------------------------------ */
/* PARAMETERS OF FUNCTION RULES                                             */
/* ------------------------------------------------------------------------ */

void parser_params() {
    debug_entry();

    parser_match(LPAREN);

    switch (lookahead.type) {
        case IDENT:
            parser_param();
            parser_match(RPAREN);
            break;
        case RPAREN:
            parser_match(RPAREN);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_param() {
    debug_entry();

    char *id = lookahead.attribute.sym_key;
    parser_match(IDENT);

    token_type type = lookahead.type;
    parser_type();

    symtable_add_func_param(st, id, type);

    parser_param_n();
}

void parser_param_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        parser_track_ident(lookahead.attribute.sym_key);
        symtable_add_func_param(st, lookahead.attribute.sym_key, lookahead.type);
        no_eol = false;
        parser_match(IDENT);

        no_eol = true;
        parser_type();
        parser_param_n();
    }
    // apply eps-rule
}

void parser_r_params() {
    debug_entry();
    // if '(' not found, apply eps-rule
    if (lookahead.type == LPAREN) {
        parser_match(LPAREN);

        switch (lookahead.type) {
            // depending on the case either put ')' or continue with params
            case INT:
            case FLOAT64:
            case STRING:
                parser_r_param();
                parser_match(RPAREN);
                break;
            case RPAREN:
                parser_match(RPAREN);
                break;
            default:
                // no eps-rule -> throw syntax error
                parser_end(ERROR_SYNTAX);
                break;
        }
    }
    // apply eps-rule
}

void parser_r_param() {
    debug_entry();

    symtable_add_func_return(st, lookahead.type);
    parser_type();
    parser_r_param_n();
}

void parser_r_param_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);
        symtable_add_func_return(st, lookahead.type);

        parser_type();
        no_eol = true;
        parser_r_param_n();
    }
    // apply eps-rule
}

void parser_func_call(char *id) {
    debug_entry();

    symtable_new_scope(st_called_funcs, id);

    // Assemble all parameters
    func_parameter_t *first_parameter = symtable_get_func_param(st_called_funcs);;
    parser_c_params(first_parameter);
}

void parser_c_params(func_parameter_t *first_param) {
    debug_entry();
    parser_match(LPAREN);

    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_c_param(first_param);
            parser_match(RPAREN);
            break;
        case RPAREN:
            parser_match(RPAREN);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_c_param(func_parameter_t *param) {
    debug_entry();
    // Resolve expression
    parser_expr();
    param->next_param = func_parameter_new();
    if (param->next_param == NULL)
        parser_end(ERROR_INTERNAL);

    parser_c_param_n(param->next_param);
}

void parser_c_param_n(func_parameter_t *param) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        param->next_param = func_parameter_new();
        if (param->next_param == NULL)
            parser_end(ERROR_INTERNAL);

        // Resolve expression
        parser_expr();
        parser_c_param_n(param->next_param);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(char *id) {
    debug_entry();

    parser_track_ident(id);
    parser_match(DEFINE);

    // TODO: Resolve expression
    parser_expr();
}

void parser_assign(char *id) {
    debug_entry();

    int num_of_assigns = 0;

    if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
        fprintf(stderr, "Variable %s is not defined\n", id);
        parser_end(ERROR_SEM_VAR);
    }

    if (strcmp(id, "_") != 0)
        num_of_assigns++;

    parser_id_n(&num_of_assigns);
    parser_match(ASSIGN);
    parser_exprs(&num_of_assigns);

    // TODO: Resolve expressions

    if (num_of_assigns != 0) {
        fprintf(stderr, "Mismatched number of assigned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }
}

void parser_id_n(int *num_of_ids) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        char *id = lookahead.attribute.sym_key;
        if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
            fprintf(stderr, "Variable %s is not defined\n", id);
            parser_end(ERROR_SEM_VAR);
        }

        if (strcmp(id, "_") != 0)
            (*num_of_ids)++;

        parser_match(IDENT);
        parser_id_n(num_of_ids);
    }
    // apply eps-rule
}

void parser_exprs(int *num_of_exprs) {
    debug_entry();

    parser_expr();
    (*num_of_exprs)--;

    parser_expr_n(num_of_exprs);
}

void parser_expr_n(int *num_of_exprs) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);
        parser_expr();
        (*num_of_exprs)--;
        parser_expr_n(num_of_exprs);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* HELPER FOR RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_optdef() {
    debug_entry();
    // if IDENT not found, apply eps-rule
    if (lookahead.type == IDENT) {
        char *id = lookahead.attribute.sym_key;

        parser_match(IDENT);
        parser_vardef(id);
    }
    // apply eps-rule
}

void parser_optassign() {
    debug_entry();
    // if IDENT not found, apply eps-rule
    if (lookahead.type == IDENT) {
        char *id = lookahead.attribute.sym_key;

        parser_match(IDENT);
        parser_assign(id);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* RETURN STATEMENT                                                         */
/* ------------------------------------------------------------------------ */

// return 10, 34
// return
// eps
void parser_return() {
    debug_entry();
    parser_match(RETURN);

    int num_of_returns = symtable_get_num_of_returns(st);

    parser_optexprs(&num_of_returns);

    if (num_of_returns != 0) {
        fprintf(stderr, "Wrong number of returned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }
}

void parser_optexprs(int *num_of_exprs) {
    debug_entry();
    // optional expression depending on the type read
    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_exprs(num_of_exprs);
            break;
        default:
            // apply eps-rule
            break;
    }
}

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES - BOTTOM-UP PRECEDENCE ANALYSIS                         */
/* ------------------------------------------------------------------------ */

// Define some functions needed specifically for the precedence analysis.
int stack_top(stack_int_t *stack) {
    debug_entry();
    int top = stack->top;
    while (top >= 0) {
        if (stack->array[top] != EXPR_SYMBOL && stack->array[top] != SHF) {
            // literal found
            return stack->array[top];
        } else {
            top--;
        }
    }

    // no terminal found
    return -1;  // terminals start at 0, so this is safe
}

int stack_find(stack_int_t *stack, int value) {
    int top = stack->top;
    while (top >= 0) {
        if (stack->array[top] == value) {
            // value found
            return top;
        } else {
            top--;
        }
    }

    // searched value not found
    return -1;  // token_type starts at 0, so this is safe
}

int stack_at(stack_int_t *stack, int index) {
    if (stack->top >= index) {
        return stack->array[index];
    }

    return -1;
}

void stack_shift(stack_int_t *stack, token_type terminal) {
    debug_entry();
    // resize the stack if the stack is full, we need more space
    if (stack_int_isfull(stack)) {
        stack_int_resize(stack);
    }

    int terminal_pos = stack_find(stack, terminal);
    int curr = terminal_pos + 1;
    stack->top++;  // increment the top value to compensate for appended symbol

    // rearrange everything that has been located before the terminal
    while (curr != stack->top) {
        token_type old = stack->array[curr];
        curr++;
        stack->array[curr] = old;
    }
    stack->array[terminal_pos + 1] = SHF;  // append the shift symbol
}

bool stack_reduce(stack_int_t *stack, stack_int_t *function_stack) {
    debug_entry();
    // if '<y' is on top of the stack and rule exists
    int top = stack_int_peek(stack);

    // get the positions of the top symbol and the shift symbol
    int top_pos = stack_find(stack, top);
    int shift_pos = stack_find(stack, SHF);
    if (shift_pos == -1 || shift_pos > top_pos) return false;

    stack_int_t *rule_stack = stack_int_init();
    // determine if any other symbols are placed between these two
    for (int i = shift_pos + 1; i <= top_pos; i++) {
        int symbol = stack_at(stack, i);
        stack_int_push(rule_stack, symbol);
    }

    // check the rules
    if (!prec_apply_rule(rule_stack, function_stack)) {
        stack_int_free(rule_stack);
        return false;
    }
    stack_int_free(rule_stack);

    // actually change the symbols to E
    stack->top = shift_pos;
    stack->array[stack->top] = EXPR_SYMBOL;

    return true;
}

// Precedence table definition.
const token_type PREC_TABLE[PREC_TABLE_OP_COUNT][PREC_TABLE_OP_COUNT] =
{      //   ,   REL  ADD  MUL   (    )   id   lit   $
/*  ,  */ {EQQ, SHF, SHF, SHF, SHF, EQQ, SHF, SHF, INV},
/* REL */ {RED, RED, SHF, SHF, SHF, RED, SHF, SHF, RED},
/* ADD */ {RED, RED, RED, SHF, SHF, RED, SHF, SHF, RED},
/* MUL */ {RED, RED, RED, RED, SHF, RED, SHF, SHF, RED},
/*  (  */ {EQQ, SHF, SHF, SHF, SHF, EQQ, SHF, SHF, INV},
/*  )  */ {RED, RED, RED, RED, INV, RED, INV, INV, RED},
/* id  */ {RED, RED, RED, RED, EQQ, RED, INV, INV, RED},
/* lit */ {RED, RED, RED, RED, INV, RED, INV, INV, RED},
/*  $  */ {SHF, SHF, SHF, SHF, SHF, INV, SHF, SHF, INV}
};

int prec_get_table_index(token_type t) {
    if (t == COMMA) {
        return 0;
    } else if (is_relop(t)) {
        return 1;
    } else if (is_addop(t)) {
        return 2;
    } else if (is_mulop(t)) {
        return 3;
    } else if (t == LPAREN) {
        return 4;
    } else if (t == RPAREN) {
        return 5;
    } else if (t == IDENT) {
        return 6;
    } else if (token_is_lit(t)) {
        return 7;
    } else if (END_OF_INPUT) {
        return 8;
    } else {
        // will probably never throw
        throw_syntax_error(t, line);
    }
}

/**
 * --- basic ---
 * E : id
 * E : lit
 * E : ( E )
 * E : E REL E
 * E : E ADD E
 * E : E MUL E
 * --- FUNC  ---
 * E : id ( )
 * E : id ( E )
 * E : id ( E , ... )
 */
bool prec_apply_rule(stack_int_t *rule_stack, stack_int_t *function_stack) {
    token_type curr;  // current symbol
    NEXT_SYMBOL;

    // switch to determine the rightmost symbol
    switch (curr) {
        case IDENT:
            // E : id
            return true;
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
            // E : lit
            return true;
        case RPAREN:
            // E : ( E )
            // E : id ( )
            // E : id ( E )
            // E : id ( ... , E )
            return prec_rule_rparen(rule_stack, function_stack);
        case EXPR_SYMBOL:
            // E : E REL E
            NEXT_SYMBOL;

            if (!is_relop(curr) && !is_addop(curr) && !is_mulop(curr)) {
                return false;
            }

            NEXT_SYMBOL;
            if (curr != EXPR_SYMBOL) return false;
            return true;
        default:
            return false;
    }
}

bool prec_rule_rparen(stack_int_t *rule_stack, stack_int_t *function_stack) {
    token_type curr;
    // go to the next symbol
    NEXT_SYMBOL;
    // test the symbol
    if (curr != EXPR_SYMBOL && curr != LPAREN) return false;

    if (curr == LPAREN) {
        // E : id ( )
        NEXT_SYMBOL;
        if (curr != IDENT) return false;

        stack_int_pop(function_stack);
        return true;
    }
    // curr == E
    NEXT_SYMBOL;
    if (curr != LPAREN && curr != COMMA) return false;

    if (curr == LPAREN) {
        if (stack_int_isempty(rule_stack)) {
            // E : ( E )
            return true;
        } else {
            // E : id ( E )
            NEXT_SYMBOL;
            if (curr != IDENT) return false;

            stack_int_pop(function_stack);
            return true;
        }
    }

    // curr == ,
    // E : id ( ... , E )
    while (!stack_int_isempty(rule_stack)) {
        NEXT_SYMBOL;
        if (curr != EXPR_SYMBOL) return false;

        NEXT_SYMBOL;
        if (curr != COMMA) break;
    }
    if (curr != LPAREN) return false;

    NEXT_SYMBOL;
    if (curr != IDENT) return false;

    stack_int_pop(function_stack);
    return true;
}

void prec_clean(stack_int_t *stack, stack_int_t *func_stack) {
    stack_int_free(stack);
    stack_int_free(func_stack);
}

void parser_precedence_analysis() {
    debug_entry();

    stack_int_t *stack = stack_int_init();
    // put $ at the top of the stack
    stack_int_push(stack, END_OF_INPUT);

    token_type input = lookahead.type;
    stack_int_t *function_stack = stack_int_init();
    bool stop = false;

    // start the precedence algorithm
    while (true) {
        debug("got eol? %d no_eol? %d required_eol? %d", eol_encountered, no_eol, required_eol);
        debug("succesful stack? %d\n", prec_succesful_stack(stack));

        // break the loop if conditions are met
        if (prec_succesful_stack(stack) && eol_encountered) {
            break;
        } else if (stop && input == END_OF_INPUT) {
            break;
        } else if (stop) {
            input = END_OF_INPUT;
        }

        // check if the identifier is '_' -> semantic error
        if (input == IDENT && strcmp(lookahead.attribute.sym_key, "_") == 0) {
           // FIXME: properly throw semantic error
           exit(3);
        }

        // handle the symbol accordingly
        if (prec_handle_symbol(stack, input, &stop, function_stack)) {
            parser_move();
            input = lookahead.type;
        }

        // do some things depending on the prec_handle_symbol function

        // Mutiple assign statement, consider the comma as the end of the input
        if (stack_int_isempty(function_stack) && input == COMMA) {
            input = END_OF_INPUT;
        }

        if (eol_encountered) {
            token_type top = stack_int_peek(stack);
            // eol encountered, is this the end of the input?
            if (token_is_lit(top) || top == IDENT || top == RPAREN) {
                // top symbol is literal, id or ')'
                if (is_addop(input) || is_mulop(input) || is_relop(input) ||
                        input == COMMA || input == RPAREN) {
                    // if the following character is some operator, we can't break line
                    prec_clean(stack, function_stack);
                    throw_syntax_error(input, line);
                } else {
                    // otherwise, it should be the end of the input
                    input = END_OF_INPUT;
                }
            }
        }
    }

    if (!prec_succesful_stack(stack)) {
        prec_clean(stack, function_stack);
        throw_syntax_error(input, line);
    }

    stack_int_free(stack);
}

bool prec_succesful_stack(stack_int_t *stack) {
    return stack->top == 1 &&
           stack->array[1] == EXPR_SYMBOL && stack->array[0] == END_OF_INPUT;
}

bool prec_handle_symbol(stack_int_t *stack, token_type input, bool *stop, stack_int_t *function_stack) {
    debug_entry();
    // check if the token is valid for expression grammar
    // FIXME: What if the input token is EOF?
    /*if (t != COMMA && t != IDENT && t != LPAREN && t != RPAREN && t != END_OF_INPUT &&*/
            /*!token_is_lit(t) && !is_mulop(t) && !is_addop(t) && !is_relop(t)) {*/
        /*throw_syntax_error(t, line);*/
    /*}*/

    // terminal on top of the stack
    int top = stack_top(stack);
    if (top == -1) {
        // no terminal on top - weird, probably isn't needed
        prec_clean(stack, function_stack);
        throw_syntax_error(input, line);
    }

    int row = prec_get_table_index(top);
    int col = prec_get_table_index(input);
    debug("Top T: %s, Top: %s, input: %s", token_get_type_string(top),
            token_get_type_string(stack_int_peek(stack)), token_get_type_string(input));
    switch(PREC_TABLE[row][col]) {
        case EQQ:
            // '=' symbol
            // push input onto the stack and read the next symbol
            stack_int_push(stack, input);
            // special treatment for the FUNEXP
            if (top == IDENT && input == LPAREN) {
                stack_int_push(function_stack, 1);
            }

            return true;
        case SHF:
            // '<' symbol
            // change 'a' on stack to 'a<'
            stack_shift(stack, top);
            // and push input onto the stack and read the next symbol
            stack_int_push(stack, input);
            return true;
        case RED:
            // '>' symbol
            // if '<y' is on top of the stack and rule exists
            // convert '<y' to A and do semantic actions
            // else, throw syntax error
            if (!stack_reduce(stack, function_stack)) {
                prec_clean(stack, function_stack);
                throw_syntax_error(input, line);
            }
            break;
        default:
            // empty cell
            *stop = true;
            break;
    }
    return false;
}

void parser_expr() {
    debug_entry();
    // start the precedence analysis
    parser_precedence_analysis();
}

/* Helper functions */
bool is_relop(token_type type) {
    // Does token type represent relation operations?
    switch (type) {
        case LSS:
        case LEQ:
        case GTR:
        case GEQ:
        case EQL:
        case NEQ:
            return true;
        default:
            return false;
    }
}

bool is_addop(token_type type) {
    // Does token type represent plus/minus opperations?
    switch (type) {
        case ADD:
        case SUB:
            return true;
        default:
            return false;
    }
}

bool is_mulop(token_type type) {
    // Does token type represent multiply/divide operations?
    switch (type) {
        case MUL:
        case DIV:
            return true;
        default:
            return false;
    }
}

/* ------------------------------------------------------------------------ */
/* SEMANTIC ACTIONS                                                         */
/* ------------------------------------------------------------------------ */

void parser_track_ident(char *id) {
    debug_entry();
    debug("Adding symbol %s to symtable", id);

    symtable_symbol_t *symbol = symtable_add_symbol(st, id);

    symbol->type = IDENT;
}
