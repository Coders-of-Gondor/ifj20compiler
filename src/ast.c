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
    p->prev_param = NULL;
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

void func_add_parameter(func_t *f, char *id, token_type type) {
    if (f == NULL)
        return;

    func_parameter_t *new_parameter = func_parameter_new();
    if (new_parameter == NULL)
        return;

    if (id != NULL) {
       strInit(&new_parameter->id);
       for (size_t i = 0; i < strlen(id); i++)
           strAddChar(&new_parameter->id, id[i]);
    }

    new_parameter->type = type;

    if (f->first_parameter == NULL) {
        f->first_parameter = new_parameter;
    } else {
        func_parameter_t *last_parameter = f->first_parameter;
        while (last_parameter->next_param != NULL)
            last_parameter = last_parameter->next_param;
        last_parameter->next_param = new_parameter;
        last_parameter->next_param->prev_param = last_parameter;
    }
    f->num_of_parameters++;
}

void func_add_return(func_t *f, token_type type) {
    if (f == NULL)
        return;

    func_return_t *new_return = func_return_new();
    if (new_return == NULL)
        return;

    new_return->type = type;

    if (f->first_return == NULL) {
        f->first_return = new_return;
    } else {
        func_return_t *last_return = f->first_return;
        while (last_return->next_return != NULL)
            last_return = last_return->next_return;
        last_return->next_return = new_return;
    }
    f->num_of_returns++;
}

assign_t *assign_new(token_type type) {
    assign_t *a = malloc(sizeof(struct assign));
    if (a == NULL)
        return NULL;

    a->type = type;
    a->next_assign = NULL;

    return a;
}

void assign_free(assign_t *a) {
    if (a == NULL)
        return;

    free(a);
}

void func_call_init(func_call_t *fc) {
    if (fc == NULL)
        return;

    func_init(&fc->func);
    fc->num_of_expected_returns = 0;
    fc->first_assign = NULL;
}

void func_call_free(func_call_t *fc) {
    if (fc == NULL)
        return;

    func_free(&fc->func);
    while (fc->first_assign != NULL) {
        assign_t *tmp_assign = fc->first_assign->next_assign;
        assign_free(fc->first_assign);
        fc->first_assign = tmp_assign;
    }
}

void func_call_add_assign(func_call_t *fc, token_type type) {
    if (fc == NULL)
        return;

    assign_t *new_assign = assign_new(type);
    if (new_assign == NULL)
        return;

    if (fc->first_assign == NULL) {
        fc->first_assign = new_assign;
    } else {
        assign_t *last_assign = fc->first_assign;
        while (last_assign->next_assign != NULL)
            last_assign = last_assign->next_assign;
        last_assign->next_assign = new_assign;
    }
    fc->num_of_expected_returns++;
}
