/* token.h
 * Ondřej Míchal <xmicha80>
 * FIT BUT
 * 03/10/2020
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

typedef enum token_type {
  INVALID,

  // Types
  INT, // 42
  FLOAT64, // 42.42
  STRING, // "fourtytwo"

  // Control
  IF, // if
  ELSE, // else
  FOR, // for

  // Functions
  FUNC, // func
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
  OR, // ||
  EQL, // ==
  LSS, // <
  LEQ, // <=
  GTR, // >
  GEQ, // >=
} token_type;

// FIXME: attribute is quite questionable member of the token struct.
/**
 * Token holds information about a scanned lexem
 */
typedef struct token {
  token_type type; /**< Type of the token */
  void *attribute; /**< Token's attribute (e.g., name of variable, value of int,..). Needs to be casted based on token type */
} token_t;

#endif // __TOKEN_H__
