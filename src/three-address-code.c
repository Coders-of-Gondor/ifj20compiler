/**
 * @file three-address-code.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @author Ondřej Míchal <xmicha80>
 * @author Marek Filip <xfilip46>
 * @brief definition of functions that work with three adress code
 * structure. This file was inspired by c201.c file from first IAL 
 * homework, done by Vojtěch Fiala.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 2/12/2020
 */

#include <stdlib.h>

#include "debug.h"
#include "error.h"
#include "three-address-code.h"

TACList *TAC_new() {
    TACList *L = malloc(sizeof(TACList));
    if (L == NULL) {
        return NULL;
    }

    L->first = NULL;
    L->act = NULL;

    return L;
}

bool TAC_create_row(TACList *L) {
    TACptr new = malloc(sizeof(struct TAC));
    if (new == NULL) {
        return false;
    }

    //new row is the last one so the
    //pointer to next row will be set to NULL
    new->next_row = NULL;

    //if it is the first row
    //first and active will point to the same struct
    //else the new row will be added behind the active row
    //and activity will be switched to it
    if (L->first == NULL) {
        L->first = new; 
        L->act = new;
    } else {
        L->act->next_row = new;
        L->act = new;
    }

    return true;
}

bool TAC_insert(TACList *L, operator_type op_type, item arg1, item arg2, item result) {
    debug_entry();
    L->act->op = op_type;
    L->act->arg1 = arg1;
    L->act->arg2 = arg2;
    L->act->result = result;

    if(!TAC_create_row(L)) {
        return false;
    }
    debug("TAC - %d - '%s', '%s' : '%s'", op_type, arg1, arg2, result);

    return true;
}

void TAC_free(TACList *L) {
    TACptr temp = L->first;

    while(temp != NULL) {
        
        TACptr next = temp->next_row;

        free(temp);

        temp = next;
    }   

    free(L);
}

item TAC_create_function_argument(int arg_number) {
    char *number = convert_int_to_string(arg_number);
    item arg = "&arg";
    arg = append(arg, number);
    return arg;
}

item TAC_create_return_value(int ret_number) {
    char *number = convert_int_to_string(ret_number);
    item ret = "&retval";
    ret = append(ret, number);
    return ret;
}

item TAC_get_float(double val) {
    item float_repr = convert_double_to_string(val);
    float_repr = prepend(float_repr, "f");

    return float_repr;
}

item TAC_get_int(int64_t val) {
    item float_repr = convert_double_to_string(val);
    float_repr = prepend(float_repr, "i");

    return float_repr;
}

item TAC_get_string(string val) {
    item string_repr = val.str;
    string_repr = prepend(string_repr, "s");
    return string_repr;
}

item TAC_get_identifier(char *val) {
    item ident_repr = val;
    ident_repr = prepend(ident_repr, "d");
    return ident_repr;
}

item TAC_convert_term(token_t token) {
    switch (token.type) {
        case IDENT:
            return TAC_get_identifier(token.attribute.sym_key);
        case INT_LIT:
            return TAC_get_int(token.attribute.int_val);
        case FLOAT64_LIT:
            return TAC_get_float(token.attribute.float_val);
        case STRING_LIT:
            return TAC_get_string(token.attribute.str_val);
        default:
            return NULL;
    }
}
