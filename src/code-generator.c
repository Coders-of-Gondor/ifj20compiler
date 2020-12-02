/**
 * @file code-generator.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>

#include "code-generator.h"
#include "three-adress-code.h"
#include "error.h"
#include "global.h"

#define SPACE 32
#define HASH 35
#define BACKSLASH 92

#include "stack_stack_charptr_tptr.h"

typedef stack_stack_charptr_tptr_t stack_of_stacks;

char *conversion(char *str) {

    int length = strlen(str);
    char buff[5];
    char *new = malloc((4*length+1)*sizeof(char));
    strcpy(new, "");

    if (new == NULL) 
        exit(99);

    for (int i = 0; i < length; i++) {
        if (str[i] <= SPACE || str[i] == HASH || str[i] == BACKSLASH) {     // <= Space means that everything below space is to be replaced
            sprintf(buff, "\\%d", str[i]);
            new = strncat(new, buff, 4);
        } else {
            new = strncat(new, &str[i], 1);
        }
    }
    return new;

}

char *remove_type(char *str) {

    int length = strlen(str);
    char *new = malloc(length*sizeof(char));
    char new_float[200]; 
    double d;
    strcpy(new, "");

    if (new == NULL) 
        exit(99);

    for (int i = 1; i < length; i++) {
        new = strncat(new, &str[i], 1);     // copy string without the first letter of the former
    }

    char *buff = malloc((length+8)*sizeof(char));
    char *converted_str = malloc(length*sizeof(char));

    switch (str[0]) {
        case 's':
            converted_str = conversion(new);
            strcpy(buff, "string@");
            break;

        case 'f':
            free(converted_str);
            d = strtod(new, NULL);
            sprintf(new_float, "%a", d);
            strcpy(buff, "float@");
            strncat(buff, new_float, strlen(new_float));
            free(new);

            return buff;
            break;

        case 'i':
            strcpy(converted_str, new);
            strcpy(buff, "int@");
            break;
    }
    free(new);
    strncat(buff, converted_str, strlen(converted_str));
    free(converted_str);
    return buff;
}


void generate() {

    //deklarace pomocnych promenych

    //promenna ve ktere bude jeden radek 3Ak

    TACList *L = malloc(sizeof(struct TACList)); 

    TAC_init(L);

    TAC_create_row(L);

    TAC_insert(L, OP_LABEL, "main", NULL, NULL);
    TAC_insert(L, OP_DEFINE, NULL, NULL, "a");
    TAC_insert(L, OP_MOVE, "I5", NULL, "a");
    TAC_insert(L, OP_DEFINE, NULL, NULL, "b");

    #define STACK(F) stack_stack_charptr_tptr_ ## F
    //start generating code
    // stack_of_stacks *megastack = stack_stack_charptr_tptr_init();
    stack_of_stacks *megastack = STACK(init)();

    stack_charptr_t *string_stack1 = stack_charptr_init();
    stack_charptr_push(string_stack1, "foo");
    
    stack_charptr_ispresent(string_stack1, "foo", strcmp);
    char *stringus = stack_charptr_pop(string_stack1);
    if (!stack_charptr_ispresent(string_stack1, "foo", strcmp)) {
        stack_charptr_push(string_stack1, "foo");
    }

    STACK(push)(megastack, string_stack1);

    stack_charptr_free(string_stack1);
    STACK(free)(megastack);


    generate_head();

    build_in_input('s');
    build_in_input('i');
    build_in_input('f');

    build_in_float_to_int();
    build_in_int_to_float();
    build_in_len();

    printf("\n");
   
    L->act = L->first;

    while (L->act->next_row != NULL) {

        switch (L->act->op) {

        case OP_ADD:
            
            break;

        case OP_LABEL:
            printf("LABEL %s:\n",L->act->arg1);
            break;

        
        
        default:
            break;
        }

        L->act = L->act->next_row;
    }

    TAC_delete(L);
    

    /* while (there are instructions) {
        
        nacti novy radek 3AK

        zpracuj ho

    }

    */

}