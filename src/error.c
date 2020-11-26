/**
 * @file error.c
 * @author Marek Filip <xfilip46>
 * @brief Error function definitions.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"
#include "global.h"
#include "debug.h"

void error_exit(unsigned error_code, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    fputs("ERROR: ", stderr);
    vfprintf(stderr, fmt, ap);

    va_end(ap);

    // FIXME: deallocation of resources

    exit(error_code);
}

void throw_semantics_error(unsigned error_code, int line) {
    error_exit(error_code, "Semantics error at line %d\n", line);
}

void throw_syntax_error(token_type type, int line) {
    #ifdef NDEBUG
    unused(type);  // silence the unused-parameter warning
    #endif
    debug("Token got: %s\n", token_get_type_string(type));
    error_exit(ERROR_SYNTAX, "Syntax error at line %d!\n", line);
}

void throw_lex_error(int line) {
    error_exit(ERROR_LEXICAL, "Lexical error at line %d!\n", line);
}

void throw_internal_error() {
    error_exit(ERROR_INTERNAL, "Internal error!\n");
}

void success_exit() {
    fprintf(stderr, "Sucesfully compiled.\n");
    exit(SUCCESS);
}
