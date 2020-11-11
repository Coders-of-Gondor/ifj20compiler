/* token.h
 * Ondřej Míchal <xmicha80>
 * Marek Filip <xfilip46>
 * Vojtěch Fiala <xfiala61>
 * Vojtěch Bůbela <xbubel08>
 * FIT BUT
 * 09/11/2020
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>
#include "str.h"

typedef enum token_type {
  // General
  INVALID,  // Invalid token (Lexical error)
  EOL, 		// End of line
  EOF_T, 	// End of file
  IDENT,    // temp_value

  // Literals
  INT_LIT, 	   // 42
  FLOAT64_LIT, // 42.42
  STRING_LIT,  // "fourtytwo"

  // Data types
  INT, 		   // int
  FLOAT64, 	   // float64
  STRING,  	   // string

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
  NEQ, // !=
  LSS, // <
  LEQ, // <=
  GTR, // >
  GEQ, // >=

  //  Other
  LPAREN, 	// (
  LBRACE, 	// {
  RPAREN, 	// )
  RBRACE, 	// }
  COMMA,  	// ,
  SEMICOLON // ;
} token_type;

/**
 * @brief Token holds information about a scanned lexem
 */
typedef struct token {
  token_type type; /**< Type of the token */
  union Attribute {
	int64_t int_val;
	double float_val;
	string str_val;
	char *sym_key;	
  } attribute; /**< Token's attribute (e.g., symtable key, value of int,..). */
} token_t;

#endif // __TOKEN_H__
