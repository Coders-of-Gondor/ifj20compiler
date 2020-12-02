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

#include "ast.h"
#include "scanner.h"
#include "stack_int.h"
#include "token.h"

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
void parser_param_n();

// return types
void parser_r_params();
void parser_r_param();
void parser_r_param_n();

// function call arguments
void parser_c_params(func_parameter_t *first_param);
void parser_c_param(func_parameter_t *param);
void parser_c_param_n(func_parameter_t *param);

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(char *id);
void parser_assign(char *id);
void parser_id_n(int *num_of_ids);
void parser_exprs(int *num_of_exprs);
void parser_expr_n(int *num_of_exprs);

/* ------------------------------------------------------------------------ */
/* HELPER FOR RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_optdef();
void parser_optassign();

/* ------------------------------------------------------------------------ */
/* RETURN STATEMENT                                                         */
/* ------------------------------------------------------------------------ */

// return 10, 34
// return
// eps
void parser_return();
void parser_optexprs(int *num_of_exprs);

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES - BOTTOM-UP PRECEDENCE ANALYSIS                         */
/* ------------------------------------------------------------------------ */

/**
 * Expressions are parsed using the bottom-up method. We use precdence table
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
    if (!stack_int_isempty(rule_stack)) curr = stack_int_pop(rule_stack); \
    else {debug("empty rule stack!"); return false;} \
} while(0)

/**
 * @brief Determine the rule validity
 * @return True if the rule is applicable, false if it is not.
 */
bool prec_apply_rule(stack_int_t *rule_stack, stack_int_t *function_stack);


bool prec_rule_rparen(stack_int_t *rule_stack, stack_int_t *function_stack);

/**
 * @brief Determine if the precedence analysis constructed a single exprresion.
 * @return True if it did, false if it did not.
 */
bool prec_succesful_stack(stack_int_t *stack);

/**
 * @brief Handle the symbol according to the precedence's algorithm.
 * @return True if the parser should fetch the next symbol, false if not.
 */
bool prec_handle_symbol(stack_int_t *stack, token_type input, bool *stop, stack_int_t *function_stack);

/**
 * @brief Clean up the memory from the allocated stacks.
 */
void prec_clean(stack_int_t *stack, stack_int_t *func_stack);

/**
 * @brief Same as stack_int_peek but skip the nonterminal values.
 */
int stack_top(stack_int_t *stack);

/**
 * @brief Change the top terminal to terminal+symbol.
 * @details Every other nonterminal before the terminal will be reordered.
 */
void stack_shift(stack_int_t *stack, token_type terminal);

/**
 * @brief Change the top symbol+terminal to nonterminal rule.
 * @return True if the reduction was succesful, False if not.
 */
bool stack_reduce(stack_int_t *stack, stack_int_t *function_stack);

/**
 * @brief Find the first index position of the passed value.
 * @return Position index if found, -1 if not found.
 */
int stack_find(stack_int_t *stack, int value);

/**
 * @brief Return the value located at given index.
 * @return Value if found, -1 if not found.
 */
int stack_at(stack_int_t *stack, int index);

/**
 * @brief Start the parsing of an expression. Pass control to bottom-up.
 * @details Outdated function kept from the Expression LL analysis.
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

#endif // __PARSER_H__
