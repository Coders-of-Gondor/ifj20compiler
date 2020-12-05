    /**
 * @file three-adress-code.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief definition of functions that work with three adress code
 * structure. Tento soubor je inspirován souborem c201.c z prvího
 * domácího úkolu předmětu IAL, který vypracoval Vojtěch Fiala <xfiala61>.
 * Pro potřeby tohoto projektu je pak Vojtěch Bůbela <xbubel08> upravil.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 2/12/2020
 */

#include <stdlib.h>
#include <string.h>

#include "three-adress-code.h"
#include "error.h"

void TAC_init(TACList *L) {
    L->first = NULL;
    L->act = NULL;
}

void TAC_create_row(TACList *L) {

    TACptr new = malloc(sizeof(struct TAC));

    if (new == NULL) {
        throw_internal_error();
    }

    if (L->first == NULL) { //if its the first row
        L->first = new;
        L->act = new;
    } else {
        L->act->next_row = new; //last row now points to the new row
        L->act = new;           //new row becomes the last row
    }

    new->next_row = NULL;       //new row points to null
}

void TAC_insert(TACList *L, operator_type op_type, char *arg1, char *arg2, char *result) {

    //guess this is not needed?
    
    // if (result != NULL) {
    //     L->act->result = malloc(strlen(result) * __CHAR_BIT__);
    // }

    // if (arg1 != NULL) {
    //     L->act->arg1 = malloc(strlen(arg1) * __CHAR_BIT__);
    // }

    // if (arg2 != NULL) {
    //     L->act->arg2 = malloc(strlen(arg2) * __CHAR_BIT__);
    // }

    L->act->op = op_type;
    L->act->arg1 = arg1;
    L->act->arg2 = arg2;
    L->act->result = result;

    TAC_create_row(L);
}

void TAC_delete(TACList *L) {

    TACptr temp = L->first;

    while(temp != NULL) {
        
        TACptr next = temp->next_row;


        // if (temp->arg1 != NULL) {
        //     free(temp->arg1);
        // }

        // if (temp->arg2 != NULL)
        //     free(temp->arg2);

        // if (temp->result != NULL)
        //     free(temp->result);
            
        free(temp);

        temp = next;
    }   

    L->first = NULL;
    L->act = NULL;
}