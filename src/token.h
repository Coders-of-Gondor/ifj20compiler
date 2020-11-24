/**
 * @file token.h
 * @author Ondřej Míchal <xmicha80>
 * @author Marek Filip <xfilip46>
 * @author Vojtěch Fiala <xfiala61>
 * @author Vojtěch Bůbela <xbubel08>
 * @brief File with token type and functions definitons
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 09/11/2020
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>
#include <stdbool.h>
#include "str.h"

typedef enum token_type {
    // General
    INVALID, // Invalid token (Lexical error)
    EOL,     // End of line
    EOF_T,   // End of file
    IDENT,   // temp_value

    // Literals
    INT_LIT,     // 42
    FLOAT64_LIT, // 42.42
    STRING_LIT,  // "fourtytwo"

    // Data types
    INT,     // int
    FLOAT64, // float64
    STRING,  // string

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

    // UNARY Extension operators
    ADD_ASSIGN, // +=
    SUB_ASSIGN, // -=
    MUL_ASSIGN, // *=
    DIV_ASSIGN, // /=

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
    LPAREN,    // (
    LBRACE,    // {
    RPAREN,    // )
    RBRACE,    // }
    COMMA,     // ,
    SEMICOLON, // ;

    // Precedence analysis symbols
    END_OF_INPUT, // $ - used for precedence analysis
    EXPR_SYMBOL,  // E - used for precedence analysis

    // Precedence table symbols
    INV, // x - invalid    
    RED, // > - reduction
    SHF, // < - shift
    EQQ  // = - equals
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

void token_init(token_t *t);
void token_free(token_t *t);

void token_set_attribute(token_t *t, string str);
void token_set_type(token_t *t, token_type type);

int ident_is_keyword(string str);

token_type get_keyword_type(string str);

const char* token_get_type_string(token_type type);

/**
 * @brief Return a true/false value depending on the token's type being a literal.
 */
bool token_is_lit(token_type type);

#endif // __TOKEN_H__
