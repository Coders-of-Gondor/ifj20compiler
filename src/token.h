/* token.h
 * Ondřej Míchal <xmicha80>, Marek Filip <xfilip46>
 * FIT BUT
 * 09/11/2020
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>
#include "str.h"

typedef enum token_type {
  INVALID,
  EOL, 	 // End of line
  EOF_T, // Enf of file

  // Types
  INT, 	   // 42
  FLOAT64, // 42.42
  STRING,  // "fourtytwo"
  IDENT,   // temp_malue

  // Control
  IF,   // if
  ELSE, // else
  FOR,  // for

  // Functions
  FUNC,   // func
  RETURN, // return

  // Package declaration
  PACKAGE, // package

  // Assignment
  DEFINE, // :=
  ASSIGN, // =

  // Operators
  ADD, // +
  SUB, // -
  MUL, // *
  DIV, // /

  // Comparators
  AND, // &&
  OR,  // ||
  EQL, // ==
  LSS, // <
  LEQ, // <=
  GTR, // >
  GEQ, // >=

  //  Other
  LPAREN, 	// (
  LBRACK,	// [
  LBRACE, 	// {
  RPAREN, 	// )
  RBRACK, 	// ]
  RBRACE, 	// }
  COMMA,  	// ,
  SEMICOLON // ;
} token_type;

// FIXME: attribute is quite questionable member of the token struct.
/**
 * Token holds information about a scanned lexem
 */
typedef struct token {
  token_type type; /**< Type of the token */
  union {
	int64_t int_val;
	double float_val;
	string string_val;
	char *sym_key;	
  }; /**< Token's attribute (e.g., symtable key, value of int,..). */
} token_t;

#endif // __TOKEN_H__
