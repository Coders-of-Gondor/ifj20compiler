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
#include "global.h"
#include "debug.h"
#include "token.h"

void error_exit(unsigned error_code, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  fputs("ERROR: ", stderr);
  vfprintf(stderr, fmt, ap);

  va_end(ap);

  // FIXME: deallocation of resources

  exit(error_code);
}

void throw_syntax_error(token_type type, int line) {
  debug("Token got: %s\n", token_get_type_string(type));
  error_exit(ERROR_SYNTAX, "Syntax error at line %d!\n", line);
}

void throw_lex_error(int line) {
  error_exit(ERROR_LEXICAL, "Lexical error at line %d!\n", line);
}

void throw_internal_error(int line) {
  error_exit(ERROR_INTERNAL, "Internal error at line %d!\n", line);
}

void success_exit() {
  fprintf(stderr, "Sucesfully compiled.\n");
  exit(SUCCESS);
}
