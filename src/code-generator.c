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

#define STACK(F) stack_stack_charptr_tptr_ ## F

#include "stack_stack_charptr_tptr.h"

typedef stack_stack_charptr_tptr_t stack_of_stacks;
typedef stack_charptr_t stack_of_strings;

int get_scope(char *var) {
    int length = strlen(var);
    int occurence = 0;

    for (int i = 0; i < length; i++) {
        if (var[i] == '$')
            occurence = i;
    }
    
    char scope_part[10];
    int k = 0;
    for (int i = occurence+1; i < length; i++) {
        scope_part[k] = var[i];
        k++;
    }
    int scope = strtol(scope_part, NULL, 10);
    return scope;
}

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
    if (buff == NULL)
        exit(99);
    char *converted_str = malloc(length*sizeof(char));
        if (converted_str == NULL)
            exit(99);

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

        case 'd':
            strcpy(buff, "");
            strcpy(converted_str, new);
            break;
    }
    //free(new);    // TODO: fix this, segfaults if we have a string with a space(s)
    strncat(buff, converted_str, strlen(converted_str));
    free(converted_str);
    return buff;
}

// char* set_scope(char *var, int scope, stack_charptr_t *stack) {
//     char *buff = malloc((5+strlen(var)*sizeof(char)));
//     char append_scope[5];

//     strcpy(buff, "");
//     strncat(buff, var, strlen(var));
//     sprintf(append_scope, "$%d", scope);
//     strncat(buff, append_scope, strlen(append_scope));

//     for (int i = scope; i > 0; i--) {
//         if (stack_charptr_ispresent(stack, buff, strcmp)); // TODO
//             //Do_something
//     }
//     return buff;
// }

// char* change_scope(char *var, int scope) {
//     int length = strlen(var);
//     char *buff = malloc((5 + length)*sizeof(char));
//     strcpy(buff, "");
//     int occurence = 0;
//     char append_scope[5];
//     sprintf(append_scope, "%d", scope);

//     for (int i = 0; i < length; i++) {
//         if (var[i] == '$')
//             occurence = i;
//     }
    
//     printf("%d\n", occurence);
//     strncat(buff, var, occurence+1);
//     strncat(buff, append_scope, strlen(append_scope));

//     return buff;
// }

int get_scope(char *var) {
    int length = strlen(var);
    int occurence = 0;

    for (int i = 0; i < length; i++) {
        if (var[i] == '$')
            occurence = i;
    }
    
    char scope_part[10];
    int k = 0;
    for (int i = occurence+1; i < length; i++) {
        scope_part[k] = var[i];
        k++;
    }
    int scope = strtol(scope_part, NULL, 10);
    return scope;
}

char *assign_scope(int scope, char *var, stack_of_stacks *megastack){

    stack_of_strings *temp = stack_stack_charptr_tptr_peek(megastack);

    int temp_scope = scope;
    char *tmp_string = malloc(__CHAR_BIT__ * (strlen(var) + 5));

    do {


        sprintf(tmp_string, "%s$%d", var, temp_scope);
        temp_scope--;
        break;

    } while (!stack_charptr_ispresent(temp, tmp_string, strcmp));

    return tmp_string;
}

void generate() {
    debug_entry();
    
    // return;

    int scope = 0;



    //deklarace pomocnych promenych

    //promenna ve ktere bude jeden radek 3Ak

    TACList *L = malloc(sizeof(struct TACList)); 

    TAC_init(L);

    TAC_create_row(L);

    TAC_insert(L, OP_LABEL_FUNC, "main", NULL, NULL);
    TAC_insert(L, OP_DEFINE, NULL, NULL, "da");
    TAC_insert(L, OP_MOVE, "i5", NULL, "da");
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_INC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_DEFINE, NULL, NULL, "da");
    TAC_insert(L, OP_MOVE, "i10", NULL, "da");
    TAC_insert(L, OP_DEC_SCOPE, NULL, NULL, NULL);
    TAC_insert(L, OP_MOVE, "i10", NULL, "da");



    
    // start generating code
    // stack_of_stacks *megastack = stack_stack_charptr_tptr_init();
    stack_of_stacks *megastack = STACK(init)();

    //  create frame for func main
    stack_charptr_t *frame_global = stack_charptr_init();
    stack_stack_charptr_tptr_push(megastack, frame_global);
    
    // stack_charptr_ispresent(string_stack1, "foo", strcmp);
    // char *stringus = stack_charptr_pop(string_stack1);
    // if (!stack_charptr_ispresent(string_stack1, "foo", strcmp)) {
        // stack_charptr_push(string_stack1, "foo");
    // }

    // STACK(push)(megastack, string_stack1);

    // stack_charptr_free(string_stack1);
    // STACK(free)(megastack);

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
            if ("is_this_a_string?")    // TODO: Check if this is a string
                print_ADD(L->act->result, L->act->arg1, L->act->arg2);
            else {
                print_CONCAT(L->act->result, L->act->arg1, L->act->arg2);
            }
            break;

        case OP_SUB:
            print_SUB(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_MUL:
            print_MUL(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_DIV:
            if ("is_this_a_float?") // TODO
                print_DIV(L->act->result, L->act->arg1, L->act->arg2);
            else if ("is_this_an int?")
                print_IDIV(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_ADD_ASSIGN:
            print_ADD_ASSIGN(L->act->result, L->act->arg1);
            break;

        case OP_SUB_ASSIGN:
            print_SUB_ASSIGN(L->act->result, L->act->arg1);
            break;

        case OP_MUL_ASSIGN:
            print_MUL_ASSIGN(L->act->result, L->act->arg1);
            break;

        case OP_DIV_ASSIGN:
            if ("is_this_a_float?") // TODO
                print_DIV_ASSIGN(L->act->result, L->act->arg1);
            else if ("is_this_an_int?")
                print_IDIV_ASSIGN(L->act->result, L->act->arg1);
            break;

        case OP_DEFINE:;

            char *processed = L->act->result;
            
            stack_of_strings *temp = stack_stack_charptr_tptr_peek(megastack);

            char *tmp_string = malloc(__CHAR_BIT__ * (strlen(processed) + 5));
            sprintf(tmp_string, "%s$%d", processed, scope);

            stack_charptr_push(temp, tmp_string);

            print_DEFINE(tmp_string);
            free(tmp_string);
            break;

        case OP_AND:
            print_AND(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_OR:
            print_OR(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_EQL:
            print_EQL(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_NEQ:
            print_NEQ(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_LSS:
            print_LSS(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_LEQ:
            print_LEQ(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_GTR:
            print_GTR(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_GEQ:
            print_GEQ(L->act->result, L->act->arg1, L->act->arg2);
            break;

        case OP_CALL:
            printf("CALL %s\n", L->act->arg1);
            break;

        case OP_CREATE_FRAME:
            printf("CREATEFRAME\n");
            break;

        case OP_LABEL:
            printf("LABEL %s:\n",L->act->arg1);
            break;

        case OP_LABEL_FUNC:
            printf("LABEL %s:\n",L->act->arg1);

            //entered definition of a function -> create new frame to
            //keep track of variables declared and defined there. Then
            //push the new_frame to stack of stacks
            stack_charptr_t *frame_new = stack_charptr_init();
            stack_stack_charptr_tptr_push(megastack, frame_new);

            scope = 1;

            break;

        case OP_RETURN:
            printf("POPFRAME\n");
            stack_stack_charptr_tptr_pop(megastack);
            scope = 0;
            printf("RETURN\n");
            break;

        case OP_MOVE:;

            char *arg1 = L->act->result;
            char *arg2 = L->act->arg1;

            if (L->act->result[0] == 'd') {
                arg1 = assign_scope(scope, L->act->result, megastack);
            }

            if (L->act->arg1[0] == 'd') {
                arg2 = assign_scope(scope, L->act->arg1, megastack);
            }

            print_MOVE(arg1, arg2);
            break;

        case OP_INC_SCOPE:
                scope++;
            break;

        case OP_DEC_SCOPE:;

            //make a copy of current stack of variables
            stack_of_strings *temp_stack = stack_stack_charptr_tptr_peek(megastack);

                do {
                    //copy the name of first variable on stack
                    char *temp_string = stack_charptr_peek(temp_stack);

                    //find out to what scope it belongs to
                    int scope_pop = get_scope(temp_string);

                    //if it dont belong to current scope that is being escaped
                    //break the cycle
                    if (scope_pop != scope) {
                        break;
                    }
                    stack_charptr_pop(temp_stack); 

                } while(1);

                scope--;
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