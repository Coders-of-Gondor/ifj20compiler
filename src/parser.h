/**
 * @file parser.h
 * @author Marek Filip <xfilip46>
 * @author Ondřej Míchal <xmicha80>
 * @brief Header file for the parser.c
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>

#include "ast.h"
#include "scanner.h"
#include "stack_func_call_t.h"
#include "stack_int.h"
#include "stack_token_t.h"
#include "token.h"
#include "three-address-code.h"

// stack
typedef stack_token_t_t stack_token_t;
typedef stack_func_call_t_t stack_func_call_t;

/**
 * @brief Move the lookahead token.
 */
void parser_move();

/**
 * @brief Match the lookahead token against give token.
 * @description Report syntax error if tokens do not match.
 */
void parser_match(token_type t);

/**
 * @brief Match the lookahead token against IDENT type and match the ident name.
 * @description Report syntax error if any match fails.
 *        This will be used for checking the 'main' part of "package main".
 */
void parser_match_ident(char *ident_name);

/**
 * @brief Start the parser. This is the starting rule of the LL(1) grammar.
 */
void parser_start(scanner_t *scanner_main);

/**
 * @brief End the parser with according return code and handle semantic checks
 * at the end of input.
 */
void parser_end(int rc);

/**
 * Below can be found the parsing functions deduced from the LL(1) table.
 */
/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_prolog();
void parser_funcs();
void parser_stmts();
void parser_stmt();
void parser_id_first();

/* ------------------------------------------------------------------------ */
/* GENERAL HELP RULES                                                       */
/* ------------------------------------------------------------------------ */

void parser_type();
void parser_block();

/* ------------------------------------------------------------------------ */
/* STATEMENT RULES                                                          */
/* ------------------------------------------------------------------------ */

void parser_stmt_if();
void parser_stmt_for();

/* ------------------------------------------------------------------------ */
/* FUNCTION RULES                                             */
/* ------------------------------------------------------------------------ */

void parser_func_call(char *id);

// function parameters
void parser_params();
void parser_param();
void parser_param_n(int arg_num);

// return types
void parser_r_params();
void parser_r_param();
void parser_r_param_n();

// function call arguments
void parser_c_params(func_t *func);
void parser_c_param(func_t *func);
void parser_c_param_n(func_t *func);

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(char *id);

void parser_assign(char* id, TACList *my_list); 
void parser_add_assign(char* id, TACList *my_list);
void parser_sub_assign(char* id, TACList *my_list);
void parser_mul_assign(char* id, TACList *my_list);
void parser_div_assign(char* id, TACList *my_list);

void parser_id_n(); 
void parser_exprs();
void parser_expr_n();

/* ------------------------------------------------------------------------ */
/* HELPER FOR RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_optdef();
void parser_optassign(TACList *my_list);

/* ------------------------------------------------------------------------ */
/* RETURN STATEMENT                                                         */
/* ------------------------------------------------------------------------ */

// return 10, 34
// return
// eps
void parser_return();
void parser_optexprs();

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES - BOTTOM-UP PRECEDENCE ANALYSIS                         */
/* ------------------------------------------------------------------------ */

/**
 * Expressions are parsed using the bottom-up method. We use precedence table
 * as our method. We need to make sure the precedences of the operators are
 * correct and that the table is constructed well. Other than that the code
 * working with our precedence table could theoretically work with a precedence
 * table of any kind. We also use the UNARY and FUNEXP extensions.
 *
 * UNARY - the difficulty here is using both +/- operators as right and left
 * associative. The solution will be to use ad hoc method to deal with
 * the issue when encountered
 * FUNEXP - the issue here is using identificators for both variables
 * and functions calls. There cannot be a '(' next to a variable, but there
 * must be one next to the function call. Another problem comes from the variable
 * argument count. I will try to deal with both issues using a smart grammar and
 * some ad hoc code.
 *
 * INV - 'x', invalid symbol - if encountered -> syntax error
 * RED - '>', reduce symbol - if '< + symbol' is on the stack and you can reduce
 *        the stack to apply a grammar rule, do so and process the semantic actions
 *        else throw a syntax error
 * SHF - '<', shift symbol - change the top symbol onto the stack ('a') to '<a'
 *        and push the input onto the stack and read the next input.
 * EQQ - '=', equal symbol - push the input onto the stack and read the next input.
 * UNA - '>|<', unary symbol - decide if to shift or to reduce depending on the stack.
 *
 * EXPRESSION RULES:
 * --- basic ---
 * E : id
 * E : lit
 * E : ( E )
 * E : E REL E
 * E : E ADD E
 * E : E MUL E
 * --- UNARY ---
 * E : ADD E
 * --- FUNC  ---
 * E : id ( )
 * E : id ( E )
 * E : id ( E , ... )
 * */

// Number of different entries row/columns in the precedence table
#define PREC_TABLE_OP_COUNT 9

/**
 * @brief The precetence table used for the parsing.
 * @details Implemented as a 2D array.
 * >|< - unary and standard
 *  --- ,   REL ADD MUL (   )   id  lit $
 *  ,   =   <   <   <   <   =   <   <
 *  REL >   >   <   <   <   >   <   <   >
 *  ADD >   >  >|<  <   <   >   <   <   >
 *  MUL >   >   >   >   <   >   <   <   >
 *  (   =   <   <   <   <   =   <   <
 *  )   >   >   >   >       >           >
 *  id  >   >   >   >   =   >           >
 *  lit >   >   >   >       >           >
 *  $   <   <   <   <   <       <   <
 *
 * REL : < | > | <= | >= | == | !=
 * ADD : + | -
 * MUL : * | /
 * id  : any identificator
 * lit : any literal
 */
extern const token_type PREC_TABLE[PREC_TABLE_OP_COUNT][PREC_TABLE_OP_COUNT];

/**
 * @brief Compute the index of the precedence table from the given token.
 * @return index
 */
int prec_get_table_index(token_type t);

// Macro to read the next symbol, so we don't have to write it over and over again
#define NEXT_SYMBOL do { \
    if (!stack_token_t_isempty(rule_stack)) { \
        curr_token = stack_token_t_pop(rule_stack); \
        curr_symbol = curr_token.type; \
        debug("Next symbol is of type %s", token_get_type_string(curr_symbol)); \
    } else { \
        debug("Empty rule stack!"); \
        return false; \
    } \
} while(0)

/**
 * @brief Determine the rule validity
 * @return false - failed -> error
 *         true - success
 */
bool prec_apply_rule(stack_token_t *rule_stack, stack_int_t *function_stack);


/**
 * @brief Helper function for handling ')' rule reduction.
 * @return success/fail
 */
bool prec_rule_rparen(stack_token_t *rule_stack, stack_int_t *function_stack);

/**
 * @brief Determine if the precedence analysis constructed a single exprresion.
 * @return True if it did, false if it did not.
 */
bool prec_succesful_stack(stack_token_t *token_stack);

/**
 * @brief Handle the symbol according to the precedence's algorithm.
 * @return True if the parser should fetch the next symbol, false if not.
 */
bool prec_handle_symbol(stack_token_t *token_stack, token_t input, bool *stop, stack_int_t *function_stack);

/**
 * @brief Clean up the memory from the allocated stacks.
 */
void prec_clean(stack_token_t *token_stack, stack_int_t *func_stack);

/**
 * @brief Same as stack_token_t_peek but skip the nonterminal values.
 * @details Token is returned as an argument reference.
 * @return true if such nonterminal found, false if not.
 */
bool stack_top_nonterminal(stack_token_t *stack, token_t *value);

/**
 * @brief Change the top terminal to terminal+symbol.
 * @details Every other nonterminal before the terminal will be reordered.
 */
void stack_shift(stack_token_t *token_stack, token_t terminal);

/**
 * @brief Change the everything between the shift and top to nonterminal rule.
 * @return True if the reduction was succesful, False if not.
 */
bool stack_reduce(stack_token_t *token_stack, stack_int_t *function_stack);

/**
 * @brief Helper function to simplify stack_token_t_find.
 * @details Not used anymore, left for backwards compatibility :D.
 */
inline int stack_find(stack_token_t_t *stack, token_t val) {
    return stack_token_t_find(stack, val, cmp_token_t);
}

// Create the expression, shift and end of input symbols.

/**
 * @brief Create the expression (E) symbol.
 */
token_t prec_create_expr_symbol();

/**
 * @brief Create the shift (<) symbol.
 */
token_t prec_create_shift_symbol();

/**
 * @brief Create the end of input ($) symbol.
 */
token_t prec_create_EOI_symbol();


/**
 * @brief Start the parsing of an expression. Pass control to bottom-up.
 * @details Outdated function kept from the Expression LL analysis.
 * @param target_type target type of the expression; different ypes may require
 * different handling
 */
void parser_expr();

/**
 * @brief Precedence analysis procedure that starts the parsing of expressions.
 * @details Needs stack implementation to work properly.
 */
void parser_precedence_analysis();

// is token type relational operator
bool is_relop(token_type type);
// is token type add/sub operator
bool is_addop(token_type type);
// is token type mul/div operator
bool is_mulop(token_type type);

/* ------------------------------------------------------------------------ */
/* SEMANTIC ACTIONS                                                         */
/* ------------------------------------------------------------------------ */

void parser_track_ident(char *id);

/**
 * clean_func_calls cleans the stack of function calls
 */
void clean_func_calls();

/* ------------------------------------------------------------------------ */
/* THREE ADDRESS CODE GENERATION ACTIONS                                    */
/* ------------------------------------------------------------------------ */

/**
 * @brief Create a new temporary variable
 * @details Prepended with $ to counter collisions with identifiers
 * @return NULL on error, temporary variable identifier on success
 */
char *create_temporary_variable(token_type type);

/**
 * @brief clean_temporary_variables cleans the stack of idents of temporary
 *        variables
 */
void clean_temporary_variables();

/**
 * @brief Create a new unique label.
 * @return NULL on error, label on success.
 */
char *create_label_name();

#endif // __PARSER_H__
