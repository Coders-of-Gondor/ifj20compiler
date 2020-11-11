/*
 * token.c
 * Vojtěch Bůbela <xbubel08>
 * Vojtech Fiala <xfiala61>
 * 09/11/2020
 */

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include "str.h"

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
        case STRING:
            strInit(&t->attribute.str);       
            strCopyString(&t->attribute.str, &str);
            break;
        case INT:
            tmp_int = atol((&str)->str);
            t->attribute.integer_value = tmp_int;
            break;
        case FLOAT64:
            tmp_d = atof((&str)->str);
            t->attribute.float_value = tmp_d;
            break;
        // TODO set symtable pointer
        default: 
            return;
    }
}


/**
 * @brief token_init initializes an instance of token
 */
void token_init(token_t *t) {
    t->type = INVALID;
    t->attribute.integer_value = 0;
}

void token_free(token_t *t) {
  if (t != NULL) {
    free(t);
  }
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
