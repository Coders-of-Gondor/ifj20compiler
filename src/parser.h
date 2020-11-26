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
/* EXPRESSION RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_expr();

// is token type relational operator
bool is_relop(token_type type); 
// is token type add/sub operator
bool is_addop(token_type type); 
// is token type mul/div operator
bool is_mulop(token_type type); 

void parser_rel(); 
void parser_rel_n(); 

void parser_add(); 
void parser_add_n(); 

void parser_term(); 
void parser_term_n(); 

void parser_factor(); 
void parser_funexp(); 

/* ------------------------------------------------------------------------ */
/* SEMANTIC ACTIONS                                                         */
/* ------------------------------------------------------------------------ */

void parser_track_ident(char *id);

#endif // __PARSER_H__
