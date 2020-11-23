/**
 * @file token.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @author Ondřej Míchal <xmicha80>
 * @brief File containing token-related functions
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 09/11/2020
 */

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"      
#include "str.h"
#include "symtable.h"
#include "debug.h"

/**
 * @brief token_set_attribute sets token attribute according to the token type
 * 
 * @param t is a pointer to token
 * 
 */
void token_set_attribute(token_t *t, string str) {
    int64_t tmp_int;
    double tmp_d;

    switch (t->type) {
        case STRING_LIT:
            strInit(&t->attribute.str_val);      
            string new;
            strInit(&new);
            char *get_string = strGetStr(&str);
            int length = strGetLength(&str);

            for (int i = 0; i < length; i++) {
                if (i > 0 && i < length-1) {
                    strAddChar(&new, get_string[i]);
                }
            }
            
            strCopyString(&t->attribute.str_val, &new);
            strFree(&new);
            break;
        case INT_LIT:
            tmp_int = atol((&str)->str);
            t->attribute.int_val = tmp_int;
            break;
        case FLOAT64_LIT:
            tmp_d = atof((&str)->str);
            t->attribute.float_val = tmp_d;
            break;
        case IDENT:
            strInit(&t->attribute.str_val);
            strCopyString(&t->attribute.str_val, &str);
            // Set token type matching the keyword
            if (ident_is_keyword(str) == 0)
                t->type = get_keyword_type(str);
            break;
        default: 
            return;
    }
}


/**
 * @brief token_init initializes an instance of token
 */
void token_init(token_t *t) {
    t->type = INVALID;
    t->attribute.int_val = 0;
}

void token_free(token_t *t) {
    if (t != NULL)
        free(t);
}

/**
 * @brief token_set_type sets type for token_t *t
 * 
 * @param t is a pointer to token
 * 
 * @param token_type is used to set type of token which is enum
 */
void token_set_type(token_t *t, token_type type) {
    t->type = type;
}

int ident_is_keyword(string str) {
    symtable_iterator_t it = symtable_find(keywords_symtable, str.str);
    if (symtable_iterator_valid(it)) {
        return 0;
    } else {
        return 1;
    }
}

token_type get_keyword_type(string str) {
    symtable_iterator_t it = symtable_find(keywords_symtable, str.str);
    symtable_value_t val = symtable_iterator_get_value(it);
    return val.type;
}

const char* token_get_type_string(token_type type) {
    switch (type) {
        case INVALID: return "INVALID";
        case EOL: return "EOL";
        case EOF_T: return "EOF_T";
        case IDENT: return "IDENT";

        // Literals
        case INT_LIT: return "INT_LIT";
        case FLOAT64_LIT: return "FLOAT64_LIT";
        case STRING_LIT: return "STRING_LIT";

        // Data types
        case INT: return "INT";
        case FLOAT64: return "FLOAT64";
        case STRING: return "STRING";

        // Control
        case IF: return "IF";
        case ELSE: return "ELSE";
        case FOR: return "FOR";

        // Functions
        case FUNC: return "FUNC";
        case RETURN: return "RETURN";

        // Package declaration
        case PACKAGE: return "PACKAGE";

        // Assignment
        case DEFINE: return "DEFINE";
        case ASSIGN: return "ASSIGN";

        // Operators
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";

        // Unary Operators
        case ADD_ASSIGN: return "ADD_ASSIGN";
        case SUB_ASSIGN: return "SUB_ASSIGN";
        case MUL_ASSIGN: return "MUL_ASSIGN";
        case DIV_ASSIGN: return "DIV_ASSIGN";

        // Comparators
        case AND: return "AND";
        case OR: return "OR";
        case EQL: return "EQL";
        case NEQ: return "NEQ";
        case LSS: return "LSS";
        case LEQ: return "LEQ";
        case GTR: return "GTR";
        case GEQ: return "GEQ";

        //  Other
        case LPAREN: return "LPAREN";
        case LBRACE: return "LBRACE";
        case RPAREN: return "RPAREN";
        case RBRACE: return "RBRACE";
        case COMMA: return "COMMA";
        case SEMICOLON: return "SEMICOLON";
        default:
            break;
    }
    return NULL;
}

bool token_is_lit(token_type type) {
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

