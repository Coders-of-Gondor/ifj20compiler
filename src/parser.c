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
#include "debug.h"


int line = 1;

// TODO: make eol rules

void parser_move() {
  debug_entry();
  // move the lookahead
  int return_code = scanner_scan(scanner, &lookahead, &eol_encountered, &line);

  // TODO: make sure all return cases are handled here

  if (return_code == ERROR_LEXICAL) {
    throw_lex_error(line);
  } else if (return_code == ERROR_INTERNAL) {
    throw_internal_error(line);
  } else if (return_code == EOF) {
    // TODO: handle this correctly
    fprintf(stderr, "EOF encountered, stopping syntax analysis...\n");
    success_exit();
  }

  if (lookahead.type == INVALID) {
    throw_lex_error(line);
  } else if (lookahead.type == EOF_T) {
    // TODO: handle this correctly
    fprintf(stderr, "EOF encountered, stopping syntax analysis...\n");
    success_exit();
  }
}

void parser_match(token_type t) {
  debug_entry();
  if (lookahead.type == t) {
    debug("Matched type: %s", token_get_type_string(t));
    parser_move();
  } else {
    debug("Expected type: %s", token_get_type_string(t));
    debug("Got type: %s", token_get_type_string(lookahead.type));
    throw_syntax_error(t, line);
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
    throw_syntax_error(lookahead.type, line);
  }
}

/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_start() {
  debug_entry();

  // move the lookahead to the first lexeme
  parser_move();

  parser_prolog();  
  parser_funcs();
  // parser_stmts(); // old
}

void parser_prolog() {
  debug_entry();
  parser_match(PACKAGE);
  parser_match_ident("main"); 
}

void parser_funcs() {
  debug_entry();
  // if FUNC not found, apply eps-rule
  if (lookahead.type == FUNC) {
      // function decleration
      parser_match(FUNC);
      parser_match(IDENT);
      parser_params();
      parser_r_params();
      parser_block();
      parser_funcs();
  }
  // apply eps-rule
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
      break;
    default:
      // apply eps-rule
      break;
  }
}

void parser_stmt() {
  debug_entry();
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
      parser_block();
      break;
    case RETURN:
      // return statement
      parser_return();
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error(lookahead.type, line);
      break;
  }
}

void parser_id_first() {
  debug_entry();
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
      parser_c_params();
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error(lookahead.type, line);
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
      break; case FLOAT64:
      parser_match(FLOAT64);
      break;
    case STRING:
      parser_match(STRING);
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error(lookahead.type, line);
      break;
  }
}

void parser_block() {
  debug_entry();
  parser_match(LBRACE);
  parser_stmts();
  parser_match(RBRACE);
}

bool is_lit(token_type type) {
  debug_entry();
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
  debug_entry();
  parser_match(LPAREN);
  parser_params_n();
}

void parser_params_n() {
  debug_entry();
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
      throw_syntax_error(line);
      break;
  } 
}

void parser_param() {
  debug_entry();
  parser_match(IDENT);
  parser_type();
  parser_param_n();
}

void parser_param_n() {
  debug_entry();
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
  debug_entry();
  // if '(' not found, apply eps-rule
  if (lookahead.type == LPAREN) {
    parser_match(LPAREN);
    parser_r_params_n();
  } 
  // apply eps-rule
}

void parser_r_params_n() {
  debug_entry();
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
      throw_syntax_error(lookahead.type, line);
      break;
  }
}

void parser_r_param() {
  debug_entry();
  parser_type();
  parser_r_param_n();
}

void parser_r_param_n() {
  debug_entry();
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_type();
    parser_r_param_n();
  }
  // apply eps-rule
}

void parser_c_params() {
  debug_entry();
  parser_match(LPAREN);
  parser_c_params_n();
}

void parser_c_params_n() {
  debug_entry();
  switch (lookahead.type) {
    case LPAREN:
    case INT_LIT:
    case FLOAT64_LIT:
    case STRING_LIT:
    case IDENT:
      parser_c_param();
      parser_match(RPAREN);
      break;
    case RPAREN:
      parser_match(RPAREN);
      break;
    default:
      // no eps-rule -> throw syntax error
      throw_syntax_error(lookahead.type, line);
      break;
  }
}

void parser_c_param() {
  debug_entry();
  parser_expr();
  parser_c_param_n();
}

void parser_c_param_n() {
  debug_entry();
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_expr();
    parser_c_param_n();
  }
  // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef() {
  debug_entry();
  parser_match(DEFINE);
  parser_expr();
}

void parser_assign() {
  debug_entry();
  parser_id_n();
  parser_match(ASSIGN);
  parser_exprs();
}

void parser_id_n() {
  debug_entry();
  // if COMMA not found, apply eps-rule
  if (lookahead.type == COMMA) {
    parser_match(COMMA);
    parser_match(IDENT);
    parser_id_n();
  }
  // apply eps-rule
}

void parser_exprs() {
  debug_entry();
  parser_expr();
  parser_expr_n();
}

void parser_expr_n() {
  debug_entry();
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
  debug_entry();
  // if IDENT not found, apply eps-rule
  if (lookahead.type == IDENT) {
    parser_match(IDENT);
    parser_vardef();
  }
  // apply eps-rule
}

void parser_optassign() {
  debug_entry();
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
  debug_entry();
  parser_match(RETURN);  
  parser_optexprs();
}

void parser_optexprs() {
  debug_entry();
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
  debug_entry();
  if (is_relop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error(lookahead.type, line);
  }
}

void parser_addop() {
  debug_entry();
  if (is_addop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error(lookahead.type, line);
  }
}

void parser_mulop() {
  debug_entry();
  if (is_mulop(lookahead.type)) {
    // we can call 'match' like that because of the check in the if 
    parser_match(lookahead.type);
  } else {
    // operator not found, syntax error
    throw_syntax_error(lookahead.type, line);
  }
}

void parser_rel() {
  debug_entry();
  parser_add();
  parser_rel_n();
}

void parser_rel_n() {
  debug_entry();
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
  debug_entry();
  parser_term();
  parser_add_n();
}

void parser_add_n() {
  debug_entry();
  // if token type is not add/sub perator, apply eps-rule
  if (is_addop(lookahead.type)) {
    parser_addop();
    parser_term();
    parser_add_n();
  }
  // apply eps-rule
}

void parser_term() {
  debug_entry();
  parser_factor();
  parser_term_n();
}

void parser_term_n() {
  debug_entry();
  // if token type is not mul/div operator, apply eps-rule
  if (is_mulop(lookahead.type)) {
    parser_mulop();
    parser_factor();
    parser_term_n();
  }
  // apply eps-rule
}

void parser_factor() {
  debug_entry();
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
    throw_syntax_error(lookahead.type, line);
  }
}

void parser_funexp() {
  debug_entry();
  // if lookahead is '(', apply eps-rule
  if (lookahead.type == LPAREN) {
    parser_c_params();
  }
  // apply eps-rule
}
