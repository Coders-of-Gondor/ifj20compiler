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

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include "token.h"
#include "../extras/htab/htab.h"

struct symtable_manager;
typedef struct symtable_manager symtable_manager_t;

symtable_manager_t *symtable_manager_new();
void symtable_manager_free(symtable_manager_t *stm);

void symtable_manager_push_stack(symtable_manager_t *stm);
void symtable_manager_pop_stack(symtable_manager_t *stm);

int symtable_manager_add_symbol(symtable_manager_t *stm, token_t *token);
void symtable_manager_remove_symbol(symtable_manager_t *stm, token_t *token);

token_t *symtable_manager_find_symbol(symtable_manager_t *stm, htab_key_t key, token_type type);


struct symtable;
typedef struct symtable symtable_t;

symtable_t *symtable_new();
void symtable_free(symtable_t *st);

void symtable_add_symbol(symtable_t *st, token_t *token);
void symtable_remove_symbol(symtable_t *st, token_t *token);

token_t *symtable_find_symbol(symtable_t *st, htab_key_t key);

#endif // __SYMTABLE_H__
