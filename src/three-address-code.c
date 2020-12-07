/**
 * @file three-address-code.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief definition of functions that work with three adress code
 * structure. This file was inspired by c201.c file from first IAL 
 * homework, done by Vojtěch Fiala.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 2/12/2020
 */

#include <stdlib.h>

#include "three-address-code.h"
#include "error.h"

void TAC_init(TACList *L) {
    L->first = NULL;
    L->act = NULL;
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

    L->act->op = op_type;
    L->act->arg1 = arg1;
    L->act->arg2 = arg2;
    L->act->result = result;

    if(!TAC_create_row(L)) {
        return false;
    }

    return true;
}

void TAC_delete(TACList *L) {
    TACptr temp = L->first;

    while(temp != NULL) {
        
        TACptr next = temp->next_row;

        free(temp);

        temp = next;
    }   

    L->first = NULL;
    L->act = NULL;
}
