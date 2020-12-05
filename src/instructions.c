/**
 * @file scanner.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Scanner DFA
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 24/11/2020
 */

#include "code-generator.h"
#include <stdio.h>

void print_MOVE(char *arg1, char *arg2, char *arg3, char *arg4, char *number) {
    if (number == NULL) {
        printf("MOVE %s@%s %s@%s\n", arg1, arg2, arg3, arg4);
    } else {
        printf("MOVE %s@%s %s\n", arg1, arg2, number);
    }
}

void print_ADD(char *var, char *sym1, char *sym2, char *number) {

    if(number == NULL)
        printf("ADD LF@%s LF@%s LF@%s\n", var, sym1, sym2);
    else {
        printf("ADD LF@%s LF@%s LF@%s\n", var, sym1, number);
    }
}

void print_CONCAT(char *var, char *sym1, char *sym2) {
        printf("CONCAT LF@%s LF@%s LF@%s\n", var, sym1, sym2);
}

void print_SUB(char *var, char *sym1, char *sym2, char *number) {
    if(number == NULL){
        printf("SUB LF@%s LF@%s LF@%s\n", var, sym1, sym2);
    } else {
        printf("SUB LF@%s LF@%s %s\n", var, sym1, number);
    }
}

void print_MUL(char *var, char *sym1, char *sym2, char *number) {
    if(number == NULL){
        printf("MUL LF@%s LF@%s LF@%s\n", var, sym1, sym2);
    } else {
        printf("MUL LF@%s LF@%s %s\n", var, sym1, number);
    }
}

void print_DIV(char *var, char *sym1, char *sym2, char *number) {
    if(number == NULL){
        printf("DIV LF@%s LF@%s LF@%s\n", var, sym1, sym2);
    } else {
        printf("DIV LF@%s LF@%s %s\n", var, sym1, number);
    }
}

void print_IDIV(char *var, char *sym1, char *sym2, char *number) {
    if(number == NULL){
        printf("IDIV LF@%s LF@%s LF@%s\n", var, sym1, sym2);
    } else {
        printf("IDIV LF@%s LF@%s %s\n", var, sym1, number);
    }
}

void print_ADD_ASSIGN(char *var, char *sym1, char *number) {
    if(number == NULL){
        printf("ADD LF@%s LF@%s LF@%s\n", var, var, sym1);
    } else {
        printf("ADD LF@%s LF@%s %s\n", var, var, number);
    }
}

void print_SUB_ASSIGN(char *var, char *sym1, char *number) {
    if(number == NULL){
        printf("SUB LF@%s LF@%s LF@%s\n", var, var, sym1);
    } else {
        printf("SUB LF@%s LF@%s %s\n", var, var, number);
    }
}

void print_MUL_ASSIGN(char *var, char *sym1, char *number) {
    if(number == NULL){
        printf("MUL LF@%s LF@%s LF@%s\n", var, var, sym1);
    } else {
        printf("MUL LF@%s LF@%s %s\n", var, var, number);
    }
}

void print_DIV_ASSIGN(char *var, char *sym1, char *number) {
    if(number == NULL){
        printf("DIV LF@%s LF@%s LF@%s\n", var, var, sym1);
    } else {
        printf("DIV LF@%s LF@%s %s\n", var, var, number);
    }
}

void print_IDIV_ASSIGN(char *var, char *sym1, char *number) {
    if(number == NULL){
        printf("IDIV LF@%s LF@%s LF@%s\n", var, var, sym1);
    } else {
        printf("IDIV LF@%s LF@%s %s\n", var, var, number);
    }
}

void print_DEFINE(char *var) {
    printf("DEFVAR LF@%s", var);
}

void print_AND(char *var, char *sym1, char *sym2) {
    printf("AND LF@%s LF@%s LF@%s", var, sym1, sym2);
}

void print_OR(char *var, char *sym1, char *sym2) {
    printf("OR LF@%s LF@%s LF@%s", var, sym1, sym2);
}

void print_EQL(char *var, char *sym1, char *sym2) {
    printf("EQ LF@%s LF@%s LF@%s", var, sym1, sym2);
}

void print_NEQ(char *var, char *sym1, char *sym2) {
    printf("EQ LF@%s LF@%s LF@%s", var, sym1, sym2);
    printf("NOT LF@%s LF@%s", var, var);
}

void print_LSS(char *var, char *sym1, char *sym2) {
    printf("LT LF@%s LF@%s LF@%s", var, sym1, sym2);
}

void print_LEQ(char *var, char *sym1, char *sym2) {
    // x <= y => y > x
    printf("GT LF@%s LF@%s LF@%s", var, sym2, sym1);
}

void print_GTR(char *var, char *sym1, char *sym2) {
    printf("GT LF@%s LF@%s LF@%s", var, sym1, sym2);
}

void print_GEQ(char *var, char *sym1, char *sym2) {
    // x >= y => y < x
    printf("LT LF@%s LF@%s LF@%s", var, sym2, sym1);
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

void build_in_substr() {

    printf("LABEL $substr\n");
    printf("PUSHFRAME\n");
    // inner variables
    printf("DEFVAR LF@$counter_increase\n");
    printf("DEFVAR LF@$length\n");
    printf("DEFVAR LF@$result\n");
    printf("DEFVAR LF@$bool_true\n");
    printf ("DEFVAR LF@$rest_of_string_length\n");
    printf("DEFVAR LF@$tmp\n");

    // return values
    printf("DEFVAR LF@retval\n");
    printf("DEFVAR LF@retval_2\n");
    
    // initialize some values
    printf("MOVE LF@$bool_true bool@true\n");
    printf("STRLEN LF@$length LF@$1\n");
    printf("SUB LF@$rest_of_string_length LF@$length LF@$2\n");

    // check correct value
    printf("GT LF@$result LF@$length LF@$2\n");
    printf("JUMPIFNEQ bad LF@$result LF@$bool_true\n");
    printf("LT LF@$result int@0 LF@$2\n");
    printf("JUMPIFNEQ bad LF@$result LF@$bool_true\n");
    printf("LT LF@$result int@0 LF@$3\n");
    printf("JUMPIFNEQ bad LF@$result LF@$bool_true\n");

    //  check if n wasn't too big, if thats the case, correct it
    printf("GT LF@$result LF@$3 LF@$rest_of_string_length\n");
    printf("JUMPIFNEQ begin LF@$result LF@$bool_true\n");
    printf("MOVE LF@$3 LF@$rest_of_string_length\n");

    printf("LABEL begin\n");
    printf("MOVE LF@$counter_increase int@1\n");
    printf("MOVE LF@retval string@\n");

    printf("MOVE LF@$length int@0\n");
    printf("ADD LF@$length LF@$3 LF@$2\n");

    printf("LABEL loop\n");
    printf("GETCHAR LF@$tmp LF@$1 LF@$2\n");            // getchar -> move 1 char from %1 string on postion @counter to retval
    printf("ADD LF@$2 LF@$2 LF@$counter_increase\n");   // counter = counter + 1
    printf("CONCAT LF@retval LF@retval LF@$tmp\n");     // retval = retval+$tmp (concatenate)

    printf("JUMPIFNEQ loop LF@$2 LF@$length\n");        // if counter != $length, continue
    printf("JUMP good\n");

    printf("LABEL bad\n");
    printf("EXIT int@1\n"); // perhaps MOVE LF@retval_2 int@1 ?

    printf("LABEL good\n");
    printf("MOVE LF@retval_2 int@0\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
}

void build_in_ord() {

    printf("LABEL $ord\n");
    printf("PUSHFRAME\n");

    // define inner values
    printf("DEFVAR LF@$retval\n");
    printf("DEFVAR LF@$retval_2\n");
    printf("DEFVAR LF@$bool_true\n");
    printf("DEFVAR LF@$result\n");
    printf("DEFVAR LF@$length\n");

    // assign values to work with
    printf("STRLEN LF@$length LF@$1\n");            // lenght = strlen(%1)
    printf("SUB LF@$length LF@$length int@1\n");   //# lenght = length-1
    printf("MOVE LF@$bool_true bool@true\n");     

    // compare if we were given correct args
    printf("GT LF@$result LF@$2 LF@$length\n");    // if %2 (i) > length-1, bad
    printf("JUMPIFEQ bad LF@$result LF@$bool_true\n");
    printf("GT LF@$result int@0 LF@$2\n");    // if 0 > LF@%2 (i), bad
    printf("JUMPIFEQ bad LF@$result LF@$bool_true\n");

    // do the thing already
    printf("STRI2INT LF@$retval LF@$1 LF@$2\n");

    printf("MOVE LF@$retval_2 int@0\n");
    printf("JUMP end\n");

    printf("LABEL bad\n");
    printf("MOVE LF@$retval_2 int@1\n");
    printf("MOVE LF@$retval int@\n");

    printf("LABEL end\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
}

void build_in_chr() {

    printf("LABEL $ord\n");
    printf("PUSHFRAME\n");

    // define inner values
    printf("DEFVAR LF@$retval\n");
    printf("DEFVAR LF@$retval_2\n");
    printf("DEFVAR LF@$bool_true\n");
    printf("DEFVAR LF@$result\n");

    //assign values to work with
    printf("MOVE LF@$bool_true bool@true\n");

    //compare if we were given correct args
    printf("GT LF@$result LF@$1 int@255\n");   // if $1 (i) > 255, bad 
    printf("JUMPIFEQ bad LF@$result LF@$bool_true\n");
    printf("GT LF@$result int@0 LF@$1\n");    // if 0 > LF@%2 (i), bad
    printf("JUMPIFEQ bad LF@$result LF@$bool_true\n");

    // do the thing 
    printf("INT2CHAR LF@$retval LF@$1\n");

    printf("MOVE LF@$retval_2 int@0\n");
    printf("JUMP end\n");

    printf("LABEL bad\n");
    printf("MOVE LF@$retval_2 int@1\n");
    printf("MOVE LF@$retval string@\n");

    printf("LABEL end\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
}
