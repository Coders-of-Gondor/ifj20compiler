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

char *conversion(char *str) {

    int length = strlen(str);
    char buff[5];
    char *new = malloc((4*length+1)*sizeof(char));
    strcpy(new, "");

    if (new == NULL) 
        throw_internal_error();

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
    double d;
    strcpy(new, "");

    if (new == NULL) 
        throw_internal_error();

    for (int i = 1; i < length; i++) {
        new = strncat(new, &str[i], 1);     // copy string without the first letter of the former
    }

    char *buff = malloc((length+8)*sizeof(char));
    if (buff == NULL)
        throw_internal_error();
    char *converted_str = malloc(length*sizeof(char));
        if (converted_str == NULL)
            throw_internal_error();

    switch (str[0]) {
        case 's':
            converted_str = conversion(new);
            strcpy(buff, "string@");
            break;

        case 'f':
            free(converted_str);
            d = strtod(new, NULL);
            size_t bufsz = snprintf(NULL, 0, "%a", d);
            char *buf = malloc(bufsz + 1);
            if (buf == NULL) {
                throw_internal_error();
            }
            snprintf(buf, bufsz + 1, "%a", d);
            strcpy(buff, "float@");
            strncat(buff, buf, strlen(buf));
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

int get_scope(const char *var) {
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

    TAC_insert(L, OP_LABEL_FUNC, NULL, NULL, "main");
    TAC_insert(L, OP_DEFINE, NULL, NULL, "da");
    TAC_insert(L, OP_MOVE, "f8.9", NULL, "da");
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
    TAC_insert(L, OP_PRINT, "i10", NULL, NULL);
    TAC_insert(L, OP_PRINT, "f3.14", NULL, NULL);
    TAC_insert(L, OP_PRINT, "sText s mezerou", NULL, NULL);
    TAC_insert(L, OP_PRINT, "da", NULL, NULL);




    
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
    build_in_substr();
    build_in_ord();

    printf("\n");
   
    L->act = L->first;

    while (L->act->next_row != NULL) {

        char *arg1 = L->act->result;
        char *arg2 = L->act->arg1;
        char *arg3 = L->act->arg2;

        if (L->act->result != NULL){
            if (L->act->result[0] == 'd') {
                    arg1 = assign_scope(scope, L->act->result, megastack);
                }
        }

        if (L->act->arg1 != NULL){
            if (L->act->arg1[0] == 'd') {
                    arg2 = assign_scope(scope, L->act->arg1, megastack);
                }
        }

        if (L->act->arg2 != NULL){
            if (L->act->arg2[0] == 'd') {
                    arg3 = assign_scope(scope, L->act->arg2, megastack);
                }
        }

        switch (L->act->op) {

            case OP_ADD:
                print_instruction("ADD", arg1, arg2, arg3);
                break;

            case OP_ADD_STRING:
                print_instruction("CONCAT", arg1, arg2, arg3);
                break; 

            case OP_SUB:
                print_instruction("SUB", arg1, arg2, arg3);
                break;

            case OP_MUL:
                print_instruction("MUL", arg1, arg2, arg3);
                break;

            case OP_DIV:
                print_instruction("DIV", arg1, arg2, arg3);
                break;

            case OP_IDIV:
                print_instruction("IDIV", arg1, arg2, arg3);
                break;

            case OP_ADD_ASSIGN:
                print_arithmetic_ASSIGN("ADD", arg1, arg2);
                break;

            case OP_SUB_ASSIGN:
                print_arithmetic_ASSIGN("SUB", arg1, arg2);
                break;

            case OP_MUL_ASSIGN:
                print_arithmetic_ASSIGN("MUL", arg1, arg2);
                break;

            case OP_DIV_ASSIGN:
                print_arithmetic_ASSIGN("DIV", arg1, arg2);
                break;

            case OP_IDIV_ASSIGN:
                print_arithmetic_ASSIGN("IDIV", arg1, arg2);
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
                print_instruction("AND", arg1, arg2, arg3);
                break;

            case OP_OR:
                print_instruction("OR", arg1, arg2, arg3);
                break;

            case OP_EQL:
                print_instruction("EQ", arg1, arg2, arg3);
                break;

            case OP_NEQ:
                print_NEQ(arg1, arg2, arg3);
                break;

            case OP_LEQ:
            case OP_GTR:
                print_instruction("GT", arg1, arg2, arg3);
                break;

            case OP_GEQ:
            case OP_LSS:
                print_instruction("LT", arg1, arg2, arg3);
                break;

            case OP_UNARY_ADD:
                print_UNARY("ADD", arg1, arg2);
                break;

            case OP_UNARY_SUB:
                print_UNARY("SUB", arg1, arg2);
                break;

            case OP_CALL:
                printf("CALL %s\n", L->act->arg1);
                break;

            case OP_CREATE_FRAME:
                printf("CREATEFRAME\n");
                break;

            case OP_LABEL:
                printf("LABEL %s:\n",L->act->result);
                break;

            case OP_LABEL_FUNC:
                printf("LABEL %s:\n",L->act->result);

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

            case OP_MOVE:
                print_MOVE(arg1, arg2);
                break;

            case OP_INC_SCOPE:
                    scope++;
                break;

            case OP_DEC_SCOPE:;

                //make a copy of current stack of variables
                stack_of_strings *temp_stack = stack_stack_charptr_tptr_peek(megastack);

                    do {
                        // copy the name of first variable on stack
                        if(stack_charptr_isempty(temp_stack)) {
                            break;
                        }

                        const char *temp_string = stack_charptr_peek(temp_stack);

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

            case OP_JUMP:
                print_JUMP(L->act->result);
                break;

            case OP_JUMPIFEQ:
                print_JUMP_instruction("JUMPIFEQ", arg1, arg2, arg3);
                break;

            case OP_JUMPIFNEQ:
                print_JUMP_instruction("JUMPIFNEQ", arg1, arg2, arg3);
                break;

            case OP_PRINT:
                print_PRINT(arg2);   // arg2 = L->act->arg1 but w/ a scope
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