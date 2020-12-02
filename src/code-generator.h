/**
 * @file code-generator.h
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

char *conversion(char *str);
void generate();
void generate_head();

void build_in_input(char type);
void build_in_float_to_int();
void build_in_int_to_float();
void build_in_len();
void build_in_substr();
void build_in_ord();
void build_in_chr();

void print_MOVE(char *arg1, char *arg2, char *arg3, char *arg4, char *number);
void print_ADD(char *var, char *sym1, char *sym2, char *number);
void print_CONCAT(char *var, char *sym1, char *sym2);
void print_SUB(char *var, char *sym1, char *sym2, char *number);
void print_MUL(char *var, char *sym1, char *sym2, char *number);
void print_DIV(char *var, char *sym1, char *sym2, char *number);
void print_IDIV(char *var, char *sym1, char *sym2, char *number);
void print_ADD_ASSIGN(char *var, char *sym1, char *number);
void print_SUB_ASSIGN(char *var, char *sym1, char *number);
void print_MUL_ASSIGN(char *var, char *sym1, char *number);
void print_DIV_ASSIGN(char *var, char *sym1, char *number);
void print_IDIV_ASSIGN(char *var, char *sym1, char *number);
void print_DEFINE(char *var);
void print_AND(char *var, char *sym1, char *sym2);
void print_OR(char *var, char *sym1, char *sym2);
void print_EQL(char *var, char *sym1, char *sym2);
void print_NEQ(char *var, char *sym1, char *sym2);
void print_LSS(char *var, char *sym1, char *sym2);
void print_LEQ(char *var, char *sym1, char *sym2);
void print_GTR(char *var, char *sym1, char *sym2);
void print_GEQ(char *var, char *sym1, char *sym2);


#endif