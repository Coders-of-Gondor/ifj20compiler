/**
 * @file ast.c
 * @author Ondřej Míchal <xmicha80>
 * @brief Abstract representations of syntactic entities
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 26/11/2020
 */

#include "str.h"
#include "token.h"

#ifndef __AST_H__
#define __AST_H__

typedef struct function_parameter {
    string id;
    token_type type;

    struct function_parameter *next_param;
} func_parameter_t;

func_parameter_t *func_parameter_new();
void func_parameter_free(func_parameter_t *p);

typedef struct function_return {
    token_type type;

    struct function_return *next_return;
} func_return_t;

func_return_t *func_return_new();
void func_return_free(func_return_t *r);

typedef struct function {
    string id;

    int num_of_parameters;
    func_parameter_t *first_parameter;

    int num_of_returns;
    func_return_t *first_return;
} func_t;

void func_init(func_t *f);
void func_free(func_t *f);
void func_set_id(func_t *f, char *id);

#endif // __AST_H__
