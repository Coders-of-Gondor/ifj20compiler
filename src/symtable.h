/*
 * Copyright (C) 2020 Ondřej Míchal

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file symtable.h
 * @author Ondřej Míchal <xmicha80> 
 * @date 20/10/2020
 */

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdbool.h>
#include <stdlib.h>
#include "ast.h"
#include "str.h"
#include "token.h"

struct symtable;
typedef struct symtable symtable_t;

typedef const char * symtable_key_t;
typedef struct symtable_symbol {
    token_type type;
    token_t token;
} symtable_symbol_t;

symtable_t *symtable_new();
void symtable_free(symtable_t *st);

symtable_symbol_t *symtable_find_symbol(symtable_t *st, symtable_key_t key);
symtable_symbol_t *symtable_add_symbol(symtable_t *st, symtable_key_t key);
void symtable_remove_symbol(symtable_t *st, symtable_key_t key);

void symtable_push_stack(symtable_t *st);
void symtable_pop_stack(symtable_t *st);

bool symtable_new_scope(symtable_t *st, symtable_key_t id);
char *symtable_get_scope_name(symtable_t *st);

bool symtable_set_current_scope(symtable_t *st, symtable_key_t id);
void symtable_set_first_scope(symtable_t *st);
bool symtable_next_scope(symtable_t *st);

bool symtable_add_func_param(symtable_t *st, symtable_key_t id, token_type type);
func_parameter_t *symtable_get_func_param(symtable_t *st);
int symtable_get_num_of_params(symtable_t *st);

void symtable_add_func_return(symtable_t *st, token_type type);
func_return_t *symtable_get_func_return(symtable_t *st);
int symtable_get_num_of_returns(symtable_t *st);

#endif // __SYMTABLE_H__
