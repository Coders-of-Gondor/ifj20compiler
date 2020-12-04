/**
 * @file ast.c
 * @author Ondřej Míchal <xmicha80>
 * @brief Abstract representations of syntactic entities
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 26/11/2020
 */

#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "debug.h"
#include "str.h"
#include "token.h"

/*
 * function_parameter_t
 */

func_parameter_t *func_parameter_new() {
    func_parameter_t *p = malloc(sizeof(struct function_parameter));
    if (p == NULL)
        return NULL;

    strInit(&p->id);
    p->next_param = NULL;
    p->type = INVALID;

    return p;
}

void func_parameter_free(func_parameter_t *p) {
    if (p == NULL)
        return;

    strFree(&p->id);
    free(p);
}

/*
 * function_return_t
 */

func_return_t *func_return_new() {
    func_return_t *r = malloc(sizeof(struct function_return));
    if (r == NULL)
        return NULL;

    r->type = INVALID;
    r->next_return = NULL;

    return r;
}

void func_return_free(func_return_t *r) {
    if (r == NULL)
        return;

    free(r);
}

/*
 * struct function
 */

void func_init(func_t *f) {
    if (f == NULL)
        return;

    strInit(&f->id);
    f->num_of_parameters = 0;
    f->num_of_returns = 0;
    f->first_parameter = NULL;
    f->first_return = NULL;
}

void func_free(func_t *f) {
    if (f == NULL)
        return;

    strFree(&f->id);
    func_parameter_t *p_tmp = f->first_parameter;
    while(f->first_parameter != NULL) {
        p_tmp = p_tmp->next_param;
        func_parameter_free(f->first_parameter);
        f->first_parameter = p_tmp;
    }
    func_return_t *r_tmp = f->first_return;
    while(f->first_return != NULL) {
        r_tmp = r_tmp->next_return;
        func_return_free(f->first_return);
        f->first_return = r_tmp;
    }
}

void func_set_id(func_t *f, char *id) {
    if (f == NULL)
        return;

    for (size_t i = 0; i < strlen(id); i++)
        strAddChar(&f->id, id[i]);
}
