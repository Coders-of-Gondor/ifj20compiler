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
    struct function_parameter *prev_param;
} func_parameter_t;

func_parameter_t *func_parameter_new();
void func_parameter_free(func_parameter_t *p);

typedef struct function_return {
    token_type type;

    struct function_return *next_return;
} func_return_t;

func_return_t *func_return_new();
void func_return_free(func_return_t *r);

/**
 * @brief func_t represents a definition of a function
 */
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
void func_add_parameter(func_t *f, char *id, token_type type);
void func_add_return(func_t *f, token_type type);

/**
 * @brief assign_t represents an assign
 * @details it holds only a type because the variable may already be lost; this
 * is mainly used in the context of checking function calls
 */
typedef struct assign {
    token_type type;

    struct assign *next_assign;
} assign_t;

assign_t *assign_new(token_type type);
void assign_free(assign_t *a);

/**
 * @brief func_call_t holds info used to check if a function call was valid
 */
typedef struct function_call {
    func_t func; /**< info about the called function */

    int num_of_expected_returns; /**< 0 - zero, 1 - one, 2 - 2 and more; used to see the context of a call */
    assign_t *first_assign; /**< first assign in the linked list of assigns */
} func_call_t;

void func_call_init(func_call_t *fc);
void func_call_free(func_call_t *fc);
void func_call_add_assign(func_call_t *fc, token_type type);

#endif // __AST_H__
