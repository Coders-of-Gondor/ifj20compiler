/**
 * @file code-generator.h
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

#include "stack_stack_charptr_tptr.h"
#include "three-adress-code.h"

#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

int get_scope(const char *var);
char *remove_type(char *str);


char *conversion(char *str);
char* set_scope(char *var, int scope, stack_charptr_t *stack);
char* change_scope(char *var, int scope);
void generate();
void generate_head();

void build_in_input(char type);
void build_in_float_to_int();
void build_in_int_to_float();
void build_in_len();
void build_in_substr();
void build_in_ord();
void build_in_chr();

void print_MOVE(char *arg1, char *arg2);
void print_DEFINE(char *var);
void print_JUMP(char *var);
void print_PRINT(char *term);
void print_NEQ(char *var, char *sym1, char *sym2);
void print_instruction(char *instruction, char *var, char *sym1, char *sym2);
void print_UNARY_FLOAT(char *instruction, char *var, char *sym1);
void print_UNARY_INT(char *instruction, char *var, char *sym1);
void print_UNARY_ADD(char *instruction, char *var, char *sym1);
void print_arithmetic_ASSIGN(char *instruction, char *var, char *sym1);
void print_JUMP_instruction(char *instruction, char *var, char *sym1, char *sym2);
void print_arithmetic_ASSIGN(char *instruction, char *var, char *sym1);

#endif
