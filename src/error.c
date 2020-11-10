/**
 * @file error.c
 * @author Marek Filip (xfilip46, Wecros), FIT BUT
 * @date 10/11/2020
 * @brief Error functions definitions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"

void error_exit(unsigned error_code, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  fputs("ERROR: ", stderr);
  vfprintf(stderr, fmt, ap);

  va_end(ap);

  // FIXME: deallocation of resources

  exit(error_code);
}

void throw_syntax_error() {
  error_exit(ERROR_SYNTAX, "Syntax error!\n");
}

void throw_lex_error() {
  error_exit(ERROR_LEXICAL, "Lexical error!\n");
}
