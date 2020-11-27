/**
 * @file code-generator.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

#include <stdio.h>
#include "code-generator.h"
#include <string.h>
#include <stdlib.h>

#define SPACE 32
#define HASH 35
#define BACKSLASH 92

char *conversion(char *str) {

    int length = strlen(str);
    char buff[5];
    char *new = malloc(4*length+1);
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

void generate() {

    //deklarace pomocnych promenych

    //promenna ve ktere bude jeden radek 3Ak

    generate_head();

    char s = 's';
    char i = 'i';
    char f = 'f';

    build_in_input(s);
    build_in_input(i);
    build_in_input(f);

    build_in_float_to_int();
    build_in_int_to_float();
    build_in_len();

    /* while (there are instructions) {
        
        nacti novy radek 3AK

        zpracuj ho

    }

    */

}

void generate_head() {
    printf(".IFJcode20\n");
    printf("JUMP main\n\n");
}

/**
 * @brief Printing functions that prints the Input instructions
 * @description The function should be something like 'y = inputs()', so we need to track the 'y' we are assigning into
 */

void build_in_input(char type) {        // type is to differentiate between inputi, inputs, inputf; *var is the var name 
    // TODO figure out how to get what frame we at (like we wanna print LF@var or GF or WTF)
    // IMPLEMENT SMTH LIKE THIS -> char *current_frame = get_frame();


    switch (type) {
        case 's':
            printf("LABEL $inputs\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@$retval1\n"); 
            printf("DEFVAR LF@$retval2\n");  
            printf("MOVE LF@$retval2 int@0\n");
            printf("READ LF@$retval1 string\n");

            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;

        case 'i':
            printf("LABEL $inputi\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@$retval1\n"); 
            printf("DEFVAR LF@$retval2\n");  
            printf("MOVE LF@$retval2 int@0\n");
            printf("READ LF@$retval1 int\n");

            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;

        case 'f':
            printf("LABEL $inputf\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@$retval1\n"); 
            printf("DEFVAR LF@$retval2\n");  
            printf("MOVE LF@$retval2 int@0\n");
            printf("READ LF@$retval1 int\n");

            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;
    }

}

/**
 * @brief Printing function to convert float to int
 * @description The function should be something like 'x = float2int(y)'
 */
void build_in_float_to_int() {

    printf("LABEL $float2int\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$retval1\n");
    printf("FLOAT2INT LF@$retval1 LF@$  1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
 * @brief Printing function to convert int to float
 * @description The function should be something like 'x = int2float(y)'
 */
void build_in_int_to_float() {

    printf("LABEL $int2float\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$retval1\n");
    printf("INT2FLOAT LF@$retval1 LF@$1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
 * @brief Printing function to get length of argument
 * @description The function should be something like 'x = len(y)'
 */
void build_in_len() { 

    printf("LABEL $len\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$retval1\n");
    printf("STRLEN LF@$retval1 LF@$1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

void build_in_substr(char *var, char *content, char *begin, char *length) {
    // TODO rewrite getting parameters from stack
    // TODO rewrite into pseudo-assembly
    // TODO get current frame
    char *current_frame = "LF";
    int begin_tmp = atoi(begin);
    int length_tmp = atoi(length);
    if (begin_tmp > strlen(content) || begin < 0)
        return;  // do i do something here? priznak chyby?
    if (length_tmp < 0)
        return;     // same here
    if (length_tmp > strlen(content)-begin_tmp)
        length_tmp = strlen(content)-begin_tmp;

    char new[length_tmp];
    int counter = 0;

    for (int i = begin_tmp; i < (begin_tmp + length_tmp); i++) {
        new[counter] = content[i];
        counter++;
    }
    printf("DEFVAR %s@%s\n", current_frame, var);  
    printf("MOVE %s@%s %s@%s\n", current_frame, var, "string", new);

    printf("LABEL $substr\n");
    printf("PUSHFRAME\n");


    printf("POPFRAME\n");
    printf("RETURN\n");
}
