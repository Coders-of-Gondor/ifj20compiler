/**
 * @file parser.c
 * @author Marek Filip (xfilip46, Wecros), FIT BUT 
 * @date 10/11/2020
 * @brief Parser part of ifj20 compiler.
 * @details Parser is the heart of the compiler and its most difficult part.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "error.h"
#include "global.h"


void parser_move() {
  // move the lookahead
  scanner_scan(scanner, &lookahead);

  // TODO: depending on the return code of scanner_scan, terminate the program

  if (lookahead.type == INVALID) {
    throw_lex_error();
  }
  if (lookahead.type == EOF_T) {
    printf("EOF encountered, stopping syntax analysis...");
  }
}

void parser_match(token_type t) {
  if (lookahead.type == t) {
    parser_move();
  } else {
    throw_syntax_error();
  }
}

void parser_match_ident(char *ident_name) {
  if (lookahead.type == IDENT && strcmp(lookahead.sym_key, ident_name)) {
    parser_move();
  } else {
    throw_syntax_error();
  }
}

/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_start() {
  parser_prolog();  
  parser_stmts();
}

void parser_prolog() {
  parser_match(PACKAGE);
  parser_match_ident("main"); 
}

void parser_stmts() {
  switch (lookahead.type) {
    case IDENT:
    case IF:
    case FOR:
    case FUNC:
      parser_stmt();
      parser_stmts();
      break;
    default:
      // apply eps-rule
      break;
  }
}

void parser_stmt() {
  switch (lookahead.type) {
    case IDENT:
      // identifier found
      parser_match(IDENT);
      parser_id_first();
      break;
    case IF:
      // if statement
      parser_match(IF);
      parser_expr();
      parser_block();
      parser_match(ELSE);
      parser_block();
      break;
    case FOR:
      // for loop
      parser_match(FOR);
      parser_optdef();
      parser_match(SEMICOLON);
      parser_expr();
      parser_match(SEMICOLON);
      parser_optassign();
      parser_match(SEMICOLON);
      parser_block();
      break;
    case FUNC:
      // function decleration
      parser_match(FUNC);
      parser_match(IDENT);
      parser_params();
      parser_r_params();
      parser_funblock();
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error();
      break;
  }
}

void parser_id_first() {
  switch (lookahead.type) {
    case DEFINE:
      // variable definition
      parser_vardef();
      break;
    case ASSIGN:
    case COMMA:
      // variable assign
      parser_assign();
      break;
    case LPAREN:
      // function call
      parser_params();
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error();
      break;
  }
}

/* ------------------------------------------------------------------------ */
/* GENERAL HELP RULES                                                       */
/* ------------------------------------------------------------------------ */

void parser_type() {
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
      throw_syntax_error();
      break;
  }
}

void parser_block() {
  parser_match(LBRACE);
  parser_stmts();
  parser_match(RBRACE);
}

bool is_lit(token_type type) {
  // is token type a literal?
  switch (type) {
    case INT_LIT:
    case FLOAT64_LIT:
    case STRING_LIT:
      return true;
    default:
      return false;
  }
}

/* ------------------------------------------------------------------------ */
/* PARAMETERS OF FUNCTION RULES                                             */
/* ------------------------------------------------------------------------ */

void parser_params() {
  parser_match(LPAREN);
  parser_params_n();
}

void parser_params_n() {
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
      throw_syntax_error();
      break;
  } 
}

void parser_param() {
  parser_match(IDENT);
  parser_type();
  parser_param_n();
}

void parser_param_n() {
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_match(IDENT);
    parser_type();
    parser_param_n();
  }
  // apply eps-rule
}

void parser_r_params() {
  // if '(' not found, apply eps-rule
  if (lookahead.type == LPAREN) {
    parser_match(LPAREN);
    parser_r_params_n();
  } 
  // apply eps-rule
}

void parser_r_params_n() {
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
      throw_syntax_error();
      break;
  }
}

void parser_r_param() {
  parser_type();
  parser_r_param_n();
}

void parser_r_param_n() {
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_type();
    parser_r_param_n();
  }
  // apply eps-rule
}

void parser_funblock() {
  parser_match(LBRACE);
  parser_stmts();
  parser_optreturn();
  parser_match(RBRACE);
}

void parser_optreturn() {
  // if RETURN not found, apply eps-rule
  if (lookahead.type == RETURN) {
    parser_return();
  }
  // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef() {
  parser_match(DEFINE);
  parser_expr();
}

void parser_assign() {
  parser_id_n();
  parser_match(ASSIGN);
  parser_exprs();
}

void parser_id_n() {
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_match(IDENT);
    parser_id_n();
  }
  // apply eps-rule
}

void parser_exprs() {
  parser_expr();
  parser_expr_n();
}

void parser_expr_n() {
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_expr();
    parser_expr_n();
  }
  // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* HELPER FOR RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_optdef() {
  // if IDENT not found, apply eps-rule
  if (lookahead.type == IDENT) {
    parser_match(IDENT);
    parser_vardef();
  }
  // apply eps-rule
}

void parser_optassign() {
  // if IDENT not found, apply eps-rule
  if (lookahead.type == IDENT) {
    parser_match(IDENT);
    parser_assign();
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
  parser_match(RETURN);  
  parser_optexprs();
}

void parser_optexprs() {
  // optional expression depending on the type read
  switch (lookahead.type) {
    case LPAREN:
    case INT_LIT:
    case FLOAT64_LIT:
    case STRING_LIT:
    case IDENT:
      parser_exprs();
      break;
    default:
      // apply eps-rule
      break;
  }
}

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_expr() {
  // This parsing is only used for the 0th submission
  parser_rel();
}

/* THIS WILL BE LEFT OUT IN THE FINAL PARSER */

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

void parser_relop() {
  if (is_relop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error();
  }
}

void parser_addop() {
  if (is_addop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error();
  }
}

void parser_mulop() {
  if (is_mulop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error();
  }
}

void parser_rel() {
  parser_add();
  parser_rel_n();
}

void parser_rel_n() {
  // if token type is not relation operator, apply eps-rule
  if (is_relop(lookahead.type)) {
      // expand the relation operator
      parser_relop();
      parser_add();
      parser_rel_n();
  }
  // apply eps-rule
}

void parser_add() {
  parser_term();
  parser_add_n();
}

void parser_add_n() {
  // if token type is not add/sub perator, apply eps-rule
  if (is_addop(lookahead.type)) {
    parser_addop();
    parser_term();
    parser_add_n();
  }
  // apply eps-rule
}

void parser_term() {
  parser_factor();
  parser_term_n();
}

void parser_term_n() {
  // if token type is not mul/div operator, apply eps-rule
  if (is_mulop(lookahead.type)) {
    parser_mulop();
    parser_factor();
    parser_term_n();
  }
  // apply eps-rule
}

void parser_factor() {
  token_type t = lookahead.type;

  if (t == LPAREN) {
    parser_match(LPAREN);
    parser_expr();
    parser_match(RPAREN);
  } else if (is_lit(t)) {
    // can only match INT, FLOAT64, STRING due to is_lit()
    parser_match(t);
  } else if (t == IDENT) {
    parser_match(IDENT);
    parser_funexp();
  } else {
    throw_syntax_error();
  }
}

void parser_funexp() {
  // if lookahead is '(', apply eps-rule
  if (lookahead.type == LPAREN) {
    parser_params();
  }
  // apply eps-rule
}
