/**
 * @file parser.h
 * @author Marek Filip (xfilip46, Wecros), FIT BUT 
 * @date 10/11/2020
 * @brief Header file for the parser.c
 * @details Parser is the heart of the compiler and its most difficult part.
 */

#ifndef __PARSER_H__
#define __PARSER_H__

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
 * 				This will be used for checking the 'main' part of "package main".
 */
void parser_match_ident(char *ident_name);

/**
 * @brief Start the parser. This is the starting rule of the LL(1) grammar.
 */
void parser_start();

/**
 * Below can be found the parsing functions deduced from the LL(1) table.
 */
/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_prolog(); 
void parser_stmts(); 
void parser_stmt(); 
void parser_id_first(); 

/* ------------------------------------------------------------------------ */
/* GENERAL HELP RULES                                                       */
/* ------------------------------------------------------------------------ */

void parser_type(); 
void parser_block(); 

/* ------------------------------------------------------------------------ */
/* PARAMETERS OF FUNCTION RULES                                             */
/* ------------------------------------------------------------------------ */

void parser_params(); 
void parser_params_n(); 
void parser_param(); 
void parser_param_n(); 

void parser_r_params();
void parser_r_params_n();
void parser_r_param();
void parser_r_param_n();

void parser_funblock(); 
void parser_optreturn(); 

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(); 
void parser_assign(); 
void parser_id_n(); 
void parser_exprs(); 
void parser_expr_n(); 

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
void parser_optexprs(); 

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_expr();


#endif // __PARSER_H__
