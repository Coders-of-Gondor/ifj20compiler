/*
 * token.c
 * Vojtěch Bůbela <xbubel08>
 * Vojtech Fiala <xfiala61>
 * 09/11/2020
 */

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include "global.h"      
#include "str.h"
#include "symtable.h"

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
            strInit(&t->attribute.str_val);       
            strCopyString(&t->attribute.str_val, &str);
            break;
        case INT:
            tmp_int = atol((&str)->str);
            t->attribute.int_val = tmp_int;
            break;
        case FLOAT64:
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

int ident_is_keyword(string str) {
  symtable_iterator_t it = symtable_find(keywords_symtable, str.str);
  if (symtable_iterator_valid(it))
    return 0;
  else
    return 1;
}

token_type get_keyword_type(string str) {
  symtable_iterator_t it = symtable_find(keywords_symtable, str.str);
  symtable_value_t val = symtable_iterator_get_value(it);
  return val.type;
}
