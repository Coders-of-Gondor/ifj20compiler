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
 * @file symtable-private.h
 * @author Ondřej Míchal <xmicha80> 
 * @date 20/10/2020
 */

#include "ast.h"
#include "symtable.h"

struct symtable_item {
    symtable_key_t key; /** */
    symtable_symbol_t symbol; /** */
    struct symtable_item *next; /** */
};

/**
 * @brief symtable_row holds symbols and reference to outer level (depth)
 * @details symtable_row is built on top of hashtable
 */
struct symtable_row {
    size_t size; /** */
    size_t arr_size; /** */
    size_t bucket_cap; /** */

    struct symtable_row *outer_row; /** */

    struct symtable_item *item_list[]; /** */
};
typedef struct symtable_row symtable_row_t;

typedef struct symtable_iterator {
    struct symtable_item *ptr; /**< pointer to a record */
    symtable_row_t *st_row; /**< pointer to a symtable row on which the iterator operates */
    size_t idb; /**< id of a bucket where the iterator is */
    size_t ide; /**< id of an entry in a bucket */
} symtable_iterator_t;

symtable_row_t *symtable_row_new();
void symtable_row_free(symtable_row_t *st_row);

size_t symtable_hash_fun(symtable_key_t key);

symtable_iterator_t symtable_find(symtable_t *st, symtable_key_t key);

void symtable_erase_symbol(symtable_iterator_t it);

symtable_iterator_t symtable_row_begin(symtable_row_t *st_row);
symtable_iterator_t symtable_row_end(symtable_row_t *st_row);
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it);

static inline int symtable_iterator_valid(symtable_iterator_t it) {
    return it.ptr != NULL;
}

static inline int symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2) {
    return it1.ptr == it2.ptr && it1.st_row == it2.st_row;
}

/**
 * @brief symtable_stack manages levels (depth) of scope (e.g. blocks)
 */
struct symtable_stack {
    size_t stack_count; /** */

    symtable_row_t *current_row; /** */
};
typedef struct symtable_stack symtable_stack_t;

symtable_stack_t *symtable_stack_new();
void symtable_stack_free(symtable_stack_t *sts);

/**
 * @brief symtable_scope manages symbols for a single scope (e.g., global,
 * function)
 */
struct symtable_scope {
    string id; /**< corresponds to a name of a function */

    size_t num_of_params; /**< number of function parameters */
    func_parameter_t *first_parameter;

    size_t num_of_ret_args; /**< number of return arguments */
    func_return_t *first_return;

    symtable_stack_t *st_stack; /**< symbol table stack */

    struct symtable_scope *next_scope; /**< next item in the linked list */
};
typedef struct symtable_scope symtable_scope_t;

symtable_scope_t *symtable_scope_new();
void symtable_scope_free(symtable_scope_t *st_scope);

/**
 * @brief symtable is the main data strcture used for all global and local symbols
 */
struct symtable {
    size_t num_of_scopes; /**< number of scopes */

    symtable_scope_t *current_scope; /**< current scope */
    symtable_scope_t *first_scope; /**< the first scope in a linked list */
};
