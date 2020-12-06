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
#include <string.h>
#include <stdbool.h>

void print_MOVE(char *arg1, char *arg2) {
    char *tmp_var = remove_type(arg1);
    char *tmp_sym1 = remove_type(arg2);
    
    bool occurence1 = false;
    int length = strlen(arg1);
    for (int i = 0; i < length; i++) {
        if (arg1[i] == '&')
            occurence1 = true;
    }

    bool occurence2 = false;
    int length2 = strlen(arg2);
    for (int i = 0; i < length2; i++) {
        if (arg2[i] == '&')
            occurence2 = true;
    }

    if (occurence1) {
        if (arg2[0] == 'd') {
            printf("MOVE TF@%s LF@%s\n", tmp_var, tmp_sym1); 
        } else if (arg2[0] != 'd') {
            printf("MOVE TF@%s %s\n", tmp_var, tmp_sym1); 
        }
    } else if (occurence2) {
        if (arg2[0] == 'd') {
            printf("MOVE LF@%s TF@%s\n", tmp_var, tmp_sym1); 
        } else if (arg2[0] != 'd') {
            printf("MOVE LF@%s %s\n", tmp_var, tmp_sym1);   
        }
    } else {
        if (arg2[0] == 'd') {
            printf("MOVE LF@%s LF@%s\n", tmp_var, tmp_sym1); 
        } else if (arg2[0] != 'd') {
            printf("MOVE LF@%s %s\n", tmp_var, tmp_sym1);  
        }
    }
}

void print_ADD(char *var, char *sym1, char *sym2) {

    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("ADD LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("ADD LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("ADD LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("ADD LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_CONCAT(char *var, char *sym1, char *sym2) {

    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("CONCAT LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("CONCAT LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("CONCAT LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("CONCAT LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_SUB(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("SUB LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("SUB LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("SUB LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("SUB LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_MUL(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("MUL LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("MUL LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("MUL LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("MUL LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_DIV(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("DIV LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("DIV LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("DIV LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("DIV LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_IDIV(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("IDIV LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("IDIV LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("IDIV LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("IDIV LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_ADD_ASSIGN(char *var, char *sym1) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("ADD LF@%s LF@%s LF@%s\n", tmp_var, tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("ADD LF@%s LF@%s %s\n", tmp_var, tmp_var, tmp_sym1);
    }
}

void print_SUB_ASSIGN(char *var, char *sym1) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("SUB LF@%s LF@%s LF@%s\n", tmp_var, tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("SUB LF@%s LF@%s %s\n", tmp_var, tmp_var, tmp_sym1);
    }
}

void print_MUL_ASSIGN(char *var, char *sym1) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("MUL LF@%s LF@%s LF@%s\n", tmp_var, tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("MUL LF@%s LF@%s %s\n", tmp_var, tmp_var, tmp_sym1);
    }
}

void print_DIV_ASSIGN(char *var, char *sym1) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("DIV LF@%s LF@%s LF@%s\n", tmp_var, tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("DIV LF@%s LF@%s %s\n", tmp_var, tmp_var, tmp_sym1);
    }
}

void print_IDIV_ASSIGN(char *var, char *sym1) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("IDIV LF@%s LF@%s LF@%s\n", tmp_var, tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("IDIV LF@%s LF@%s %s\n", tmp_var, tmp_var, tmp_sym1);
    }
}

void print_DEFINE(char *var) {
    char *tmp_var = remove_type(var);

    if (tmp_var[0] != '&') {
        printf("DEFVAR LF@%s\n", tmp_var);      // perhaps figure out the correct frame?
    } else {
        printf("DEFVAR TF@%s\n", tmp_var);
    }
}

void print_AND(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("AND LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("AND LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("AND LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("AND LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_OR(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("OR LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("OR LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("OR LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("OR LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_EQL(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);   
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("EQL LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("EQL LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("EQL LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("EQL LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_NEQ(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("EQ LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("EQ LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("EQ LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("EQ LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }

    printf("NOT LF@%s LF@%s", tmp_var, tmp_var);
}

void print_LSS(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("LT LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("LT LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("LT LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("LT LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_LEQ(char *var, char *sym1, char *sym2) {
    // x <= y => y > x
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("GT LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("GT LF@%s %s LF@%s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("GT LF@%s LF@%s %s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("GT LF@%s %s %s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
}

void print_GTR(char *var, char *sym1, char *sym2) {
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("GT LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("GT LF@%s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("GT LF@%s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("GT LF@%s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
}

void print_GEQ(char *var, char *sym1, char *sym2) {
    // x >= y => y < x
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("LT LF@%s LF@%s LF@%s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("LT LF@%s %s LF@%s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("LT LF@%s LF@%s %s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("LT LF@%s %s %s\n", tmp_var, tmp_sym2, tmp_sym1);
    }
       
}    

void print_UNARY_ADD(char *var, char *sym1) {
    // x := +y
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("ADD LF@%s int@0 LF@%s\n", tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("ADD LF@%s int@0 %s\n", tmp_var, tmp_sym1);
    }  
}  

void print_UNARY_SUB(char *var, char *sym1) {
    // x := +y
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);

    if (sym1[0] == 'd') {
        printf("SUB LF@%s int@0 LF@%s\n", tmp_var, tmp_sym1);
    }
    else if (sym1[0] != 'd') {
        printf("SUB LF@%s int@0 %s\n", tmp_var, tmp_sym1);
    }  
}  

void print_JUMP(char *var) {
    // JUMP label
    char *tmp_var = remove_type(var);
    printf("JUMP %s\n", tmp_var);
} 

void print_JUMPIFEQ(char *var, char *sym1, char *sym2) {
    // JUMPEQ label
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("JUMPIFEQ %s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("JUMPIFEQ %s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("JUMPIFEQ %s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("JUMPIFEQ %s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
} 

void print_JUMPIFNEQ(char *var, char *sym1, char *sym2) {
    // JUMPNEQ label
    char *tmp_var = remove_type(var);
    char *tmp_sym1 = remove_type(sym1);
    char *tmp_sym2 = remove_type(sym2);

    if (sym1[0] == 'd' && sym2[0] == 'd') {
        printf("JUMPIFNEQ %s LF@%s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] == 'd') {
        printf("JUMPIFNEQ %s %s LF@%s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] == 'd' && sym2[0] != 'd') {
        printf("JUMPIFNEQ %s LF@%s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
    else if (sym1[0] != 'd' && sym2[0] != 'd') {
        printf("JUMPIFNEQ %s %s %s\n", tmp_var, tmp_sym1, tmp_sym2);
    }
} 

void generate_head() {
    printf(".IFJcode20\n");
    printf("JUMP main\n\n");
}

/**
* @brief Print a build-in functions inputi(), inputf() and inputs()
*/

void build_in_input(char type) {        // type is to differentiate between inputi, inputs, inputf; *var is the var name 
    // TODO figure out how to get what frame we at (like we wanna print LF@var or GF or WTF)
    // IMPLEMENT SMTH LIKE THIS -> char *current_frame = get_frame();


    switch (type) {
        case 's':
            printf("LABEL $inputi\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@&retval1\n"); 
            printf("DEFVAR LF@&retval2\n");
            printf("READ LF@&retval1 string\n");

            printf("MOVE LF@&retval2 int@0\n");

            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;

        case 'i':
            printf("LABEL $inputi\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@&retval1\n"); 
            printf("DEFVAR LF@&retval2\n");  
            printf("READ LF@&retval1 int\n");
            printf("DEFVAR LF@$tmp\n"); 
            printf("TYPE LF@$tmp LF@&retval1\n");
            printf("DEFVAR LF@$bool_result\n"); 
            printf("JUMPIFNEQ $bad LF@$tmp string@int\n");
            printf("MOVE LF@&retval2 int@0\n");
            printf("JUMP $end\n");


            printf("LABEL $bad\n");
            printf("MOVE LF@&retval2 int@1\n");
            printf("MOVE LF@&retval1 nil@nil\n");

            printf("LABEL $end\n");
            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;

        case 'f':
            printf("LABEL $inputi\n");
            printf("PUSHFRAME\n");

            printf("DEFVAR LF@&retval1\n"); 
            printf("DEFVAR LF@&retval2\n");  
            printf("READ LF@&retval1 float\n");
            printf("DEFVAR LF@$tmp\n"); 
            printf("TYPE LF@$tmp LF@&retval1\n");
            printf("DEFVAR LF@$bool_result\n"); 
            printf("JUMPIFNEQ $bad LF@$tmp string@float\n");
            printf("MOVE LF@&retval2 int@0\n");
            printf("JUMP $end\n");


            printf("LABEL $bad\n");
            printf("MOVE LF@&retval2 int@1\n");
            printf("MOVE LF@&retval1 nil@nil\n");

            printf("LABEL $end\n");
            printf("POPFRAME\n");
            printf("RETURN\n\n");
            break;
    }

}

/**
* @brief Print a build-in function float2int()
*/
void build_in_float_to_int() {

    printf("LABEL $float2int\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@&retval1\n");
    printf("DEFVAR LF@$1\n");
    printf("MOVE LF@$1 LF@&arg1\n");

    printf("FLOAT2INT LF@&retval1 LF@$1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
* @brief Print a build-in function int2float()
*/
void build_in_int_to_float() {

    printf("LABEL $int2float\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@&retval1\n");
    printf("DEFVAR LF@$1\n");
    printf("MOVE LF@$1 LF@&arg1\n");

    printf("INT2FLOAT LF@&retval1 LF@$1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
* @brief Print a build-in function len()
*/
void build_in_len() { 

    printf("LABEL $len\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@&retval1\n");
    printf("DEFVAR LF@$1\n");
    printf("MOVE LF@$1 LF@&arg1\n");
    printf("STRLEN LF@&retval1 LF@$1\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
* @brief Print a build-in function substr()
*/
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
    printf("DEFVAR LF@$1\n");
    printf("DEFVAR LF@$2\n");
    printf("DEFVAR LF@$3\n");
    printf("MOVE LF@$1 LF@&arg1\n");
    printf("MOVE LF@$2 LF@&arg2\n");
    printf("MOVE LF@$3 LF@&arg3\n");

    // return values
    printf("DEFVAR LF@&retval\n");
    printf("DEFVAR LF@&retval2\n");
    
    // initialize some values
    printf("MOVE LF@$bool_true bool@true\n");
    printf("STRLEN LF@$length LF@$1\n");
    printf("SUB LF@$rest_of_string_length LF@$length LF@$2\n");

    // check correct value
    printf("GT LF@$result LF@$length LF@$2\n");
    printf("JUMPIFNEQ $bad LF@$result LF@$bool_true\n");
    printf("LT LF@$result int@0 LF@$2\n");
    printf("JUMPIFNEQ $bad LF@$result LF@$bool_true\n");
    printf("LT LF@$result int@0 LF@$3\n");
    printf("JUMPIFNEQ $bad LF@$result LF@$bool_true\n");

    //  check if n wasn't too big, if thats the case, correct it
    printf("GT LF@$result LF@$3 LF@$rest_of_string_length\n");
    printf("JUMPIFNEQ $begin LF@$result LF@$bool_true\n");
    printf("MOVE LF@$3 LF@$rest_of_string_length\n");

    printf("LABEL $begin\n");
    printf("MOVE LF@$counter_increase int@1\n");
    printf("MOVE LF@&retval1 string@\n");

    printf("MOVE LF@$length int@0\n");
    printf("ADD LF@$length LF@$3 LF@$2\n");

    printf("LABEL $loop\n");
    printf("GETCHAR LF@$tmp LF@$1 LF@$2\n");            // getchar -> move 1 char from %1 string on postion @counter to retval
    printf("ADD LF@$2 LF@$2 LF@$counter_increase\n");   // counter = counter + 1
    printf("CONCAT LF@&retval1 LF@&retval1 LF@$tmp\n");     // retval = retval+$tmp (concatenate)

    printf("JUMPIFNEQ $loop LF@$2 LF@$length\n");        // if counter != $length, continue
    printf("JUMP $good\n");

    printf("LABEL $bad\n");
    printf("EXIT int@1\n"); // perhaps MOVE LF@retval_2 int@1 ?

    printf("LABEL $good\n");
    printf("MOVE LF@&retval2 int@0\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}


/**
* @brief Print a build-in function ord()
*/
void build_in_ord() {

    printf("LABEL $ord\n");
    printf("PUSHFRAME\n");

    // define inner values
    printf("DEFVAR LF@&retval1\n");
    printf("DEFVAR LF@&retval2\n");
    printf("DEFVAR LF@$bool_true\n");
    printf("DEFVAR LF@$result\n");
    printf("DEFVAR LF@$length\n");
    printf("DEFVAR LF@$1\n");
    printf("MOVE LF@$1 LF@&arg1\n");
    printf("DEFVAR LF@$2\n");
    printf("MOVE LF@$2 LF@&arg2\n");

    // assign values to work with
    printf("STRLEN LF@$length LF@$1\n");            // lenght = strlen(%1)
    printf("SUB LF@$length LF@$length int@1\n");   //& lenght = length-1
    printf("MOVE LF@$bool_true bool@true\n");     

    // compare if we were given correct args
    printf("GT LF@$result LF@$2 LF@$length\n");    // if %2 (i) > length-1, bad
    printf("JUMPIFEQ $bad LF@$result LF@$bool_true\n");
    printf("GT LF@$result int@0 LF@$2\n");    // if 0 > LF@%2 (i), bad
    printf("JUMPIFEQ $bad LF@$result LF@$bool_true\n");

    // do the thing already
    printf("STRI2INT LF@&retval1 LF@$1 LF@$2\n");

    printf("MOVE LF@&retval2 int@0\n");
    printf("JUMP $end\n");

    printf("LABEL $bad\n");
    printf("MOVE LF@&retval2 int@1\n");
    printf("MOVE LF@&retval1 int@\n");

    printf("LABEL $end\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

/**
* @brief Print a build-in function chr()
*/
void build_in_chr() {

    printf("LABEL $ord\n");
    printf("PUSHFRAME\n");

    // define inner values
    printf("DEFVAR LF@&retval1\n");
    printf("DEFVAR LF@&retval2\n");
    printf("DEFVAR LF@$bool_true\n");
    printf("DEFVAR LF@$result\n");
    printf("DEFVAR LF@$1\n");
    printf("MOVE LF@$1 LF@&arg1\n");

    //assign values to work with
    printf("MOVE LF@$bool_true bool@true\n");

    //compare if we were given correct args
    printf("GT LF@$result LF@$1 int@255\n");   // if $1 (i) > 255, bad 
    printf("JUMPIFEQ $bad LF@$result LF@$bool_true\n");
    printf("GT LF@$result int@0 LF@$1\n");    // if 0 > LF@%2 (i), bad
    printf("JUMPIFEQ $bad LF@$result LF@$bool_true\n");

    // do the thing 
    printf("INT2CHAR LF@&retval1 LF@$1\n");

    printf("MOVE LF@&retval2 int@0\n");
    printf("JUMP $end\n");

    printf("LABEL $bad\n");
    printf("MOVE LF@&retval2 int@1\n");
    printf("MOVE LF@&retval1 string@\n");

    printf("LABEL $end\n");

    printf("POPFRAME\n");
    printf("RETURN\n\n");
}
