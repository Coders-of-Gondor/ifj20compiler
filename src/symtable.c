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
 * @file symtable.c
 * @author Ondřej Míchal <xmicha80> 
 * @date 20/10/2020
 * @brief Symbol table based on hashtable.
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "str.h"
#include "symtable.h"
#include "symtable-private.h"
#include "token.h"

#define ST_INIT_NUM_OF_BUCKETS 65536
#define ST_INIT_NUM_IN_BUCKET 16

/**
 * @brief symtable_hash_fun returns a hash for a hashtable key
 *
 * @param key text
 *
 * @return hash
 */
size_t symtable_hash_fun(symtable_key_t key) {
    uint32_t h = 0; // Must be 32 bits
    const unsigned char *p;

    for (p = (const unsigned char*) key; *p != '\0'; p++) {
        h = 65599 * h + *p;
    }

    return h;
}

/**
 * @brief symtable_row_new constructs a new symtable row
 *
 * @return pointer to a symtable row
 *
 * @retval NULL error
 * @retval pointer success
 */
symtable_row_t *symtable_row_new() {
    debug_entry();

    symtable_row_t *st_row = malloc(sizeof(struct symtable_row) + ST_INIT_NUM_OF_BUCKETS * sizeof(struct symtable_symbol *));
    if (st_row == NULL)
        return NULL;

    st_row->size = 0;
    st_row->arr_size = ST_INIT_NUM_OF_BUCKETS;
    st_row->bucket_cap = ST_INIT_NUM_IN_BUCKET;

    for (size_t i = 0; i < st_row->arr_size; i++)
        st_row->item_list[i] = NULL;

    return st_row;
}

/**
 * @brief symtable_row_free clears and destroys an instance of symtable row
 *
 * @param st pointer to a symtable row
 */
void symtable_row_free(symtable_row_t *st_row) {
    debug_entry();
    if (st_row != NULL) {
        symtable_row_clear(st_row);
        free(st_row);
    }
}

/**
 * @brief symtable_row_clear erases all items in a symbol table row
 *
 * @param t pointer to a symtable row
 */
void symtable_row_clear(symtable_row_t *st_row) {
    debug_entry();
    if (st_row == NULL)
        return;

    symtable_iterator_t it = symtable_row_begin(st_row);
    symtable_iterator_t next_it;

    while (symtable_iterator_valid(it)) {
        next_it = symtable_iterator_next(it);
        symtable_erase_symbol(it);
        it = next_it;
    }
}


/**
 * @brief symtable_row_begin returns a new iterator aiming at the first object in a symbol table
 *
 * @details symtable_row_begins looks for the first object in a symtable. If it's
 * not found, it returns symtable_row_end(t).
 *
 * @param t pointer to an instance of symtable
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_row_begin(symtable_row_t *st_row) {
    symtable_iterator_t it = { st_row->item_list[0], st_row, 0, 0 };

    while(it.idb < st_row->arr_size) {
        it.ptr = st_row->item_list[it.idb];

        if (symtable_iterator_valid(it))
            return it;

        it.idb++;
    }

    return symtable_row_end(st_row);
}

symtable_iterator_t symtable_row_end(symtable_row_t *st_row) {
    symtable_iterator_t it = { NULL, st_row, st_row->arr_size, st_row->bucket_cap };

    return it;
}

/**
 * @brief symtable_iterator_next returns an iterator pointing to the next record
 *
 * @param it symtable iterator
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it) {
    if (it.ptr != NULL) {
        if (it.ptr->next != NULL) {
            it.ptr = it.ptr->next;
            it.ide++;
            return it;
        }
    }

    for (++it.idb; it.idb < it.st_row->arr_size; it.idb++) {
        if (it.st_row->item_list[it.idb] != NULL) {
            it.ptr = it.st_row->item_list[it.idb];
            it.ide = 0;

            return it;
        }
    }

    return symtable_row_end(it.st_row);
}

/**
 * @brief symtable_stack_new initializes an instance of symtable_stack
 *
 * @details symtable_stack_new allocates space for a set number of symtable
 * stacks. If more space is needed, symtable_stack_push_stack will reallocate
 * the body (by doubling the maximum of stacks). The structure starts with 1
 * stack. Initial maximum number of stacks is 4.
 *
 * @return pointer to an instance of symtable stack
 *
 * @retval NULL error
 * @retval pointer success
 */
symtable_stack_t *symtable_stack_new() {
    debug_entry();
    symtable_stack_t *st_stack = malloc(sizeof(struct symtable_stack));
    if (st_stack == NULL)
        return NULL;

    st_stack->stack_count = 1;

    symtable_row_t *st_row = symtable_row_new();
    if (st_row == NULL)
        return NULL;

    st_stack->current_row = st_row;

    return st_stack;
}

/**
 * @brief symtable_stack_free destroys an instance of symtable stacks
 *
 * @param sts pointer to an instance of symtable stack
 */
void symtable_stack_free(symtable_stack_t *st_stack) {
    debug_entry();
    if (st_stack == NULL)
        return;

    while (st_stack->current_row != NULL) {
        symtable_row_t *outer_row = st_stack->current_row->outer_row;
        symtable_row_free(st_stack->current_row);
        st_stack->current_row = outer_row;
    }
    free(st_stack);
}

/**
 * @brief symtable_stack_push adds a new symtable to the stack
 *
 * @details symtable_stack_push checks before adding a stack if there's
 * enough space for it. If there is not, it reallocates the strucutre to
 * accomodate twice as many symtable stacks.
 *
 * @param sts pointer to an instance of symtable stack
 */
void symtable_push_stack(symtable_t *st) {
    debug_entry();
    if (st == NULL)
        return;

    symtable_stack_t *st_stack = st->current_scope->st_stack;

    symtable_row_t *st_row = symtable_row_new();
    if (st_row == NULL)
        return;

    st_row->outer_row = st_stack->current_row;

    st_stack->current_row = st_row;
    st_stack->stack_count++;
}

/**
 * @brief symtable_stack_pop removes the topmost symtable from the stack
 *
 * @details symtable_stack_pop pops a stack only if there's more than
 * one symtable in the stack. The popped symtable is not kept in memory.
 *
 * @param sts pointer to an instance of symtable stack
 */
void symtable_pop_stack(symtable_t *st) {
    debug_entry();
    if (st == NULL)
        return;

    symtable_stack_t *st_stack = st->current_scope->st_stack;

    if (st_stack->stack_count > 1) {
        symtable_row_t *new_row = st_stack->current_row->outer_row;

        symtable_row_free(st_stack->current_row);

        st_stack->current_row = new_row;
        st_stack->stack_count--;
    }
}

bool symtable_new_scope(symtable_t *st, symtable_key_t id) {
    debug_entry();
    if (st == NULL)
        return false;

    symtable_scope_t *st_scope = symtable_scope_new();
    if (st_scope == NULL)
        return false;

    symtable_scope_t *tmp_scope = st->first_scope;
    while (tmp_scope != NULL) {
        if (strCmpConstStr(&tmp_scope->id, (char *) id) == 0)
            return false;

        tmp_scope = tmp_scope->next_scope;
    }

    for (size_t i = 0; i < strlen(id); i++)
        strAddChar(&st_scope->id, id[i]);

    // Get the last scope in the list to append the new one
    symtable_scope_t *last_scope = st->first_scope;
    while (last_scope->next_scope != NULL)
        last_scope = last_scope->next_scope;

    last_scope->next_scope = st_scope; 
    st->current_scope = st_scope;
    st->num_of_scopes++;

    return true;
}

symtable_scope_t *symtable_scope_new() {
    symtable_scope_t *st_scope = malloc(sizeof(struct symtable_scope));
    if (st_scope == NULL)
        return NULL;

    symtable_stack_t *st_stack = symtable_stack_new();
    if (st_stack == NULL)
        return NULL;

    func_parameter_t *p = func_parameter_new();
    if (p == NULL)
        return NULL;

    func_return_t *r = func_return_new();
    if (r == NULL)
        return NULL;

    strInit(&st_scope->id);
    st_scope->num_of_params = 0;
    st_scope->num_of_ret_args = 0;
    st_scope->first_parameter = p;
    st_scope->first_return = r;
    st_scope->st_stack = st_stack;
    st_scope->next_scope = NULL;

    return st_scope;
}

void symtable_scope_free(symtable_scope_t *st_scope) {
    debug_entry();

    if (st_scope == NULL)
        return;

    symtable_stack_free(st_scope->st_stack);
    while(st_scope->first_parameter != NULL) {
        func_parameter_t *next_parameter = st_scope->first_parameter->next_param;
        func_parameter_free(st_scope->first_parameter);
        st_scope->first_parameter = next_parameter;
    }
    while(st_scope->first_return != NULL) {
        func_return_t *next_return = st_scope->first_return->next_return;
        func_return_free(st_scope->first_return);
        st_scope->first_return = next_return;
    }
    strFree(&st_scope->id);
    free(st_scope);
}

char *symtable_get_scope_name(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return NULL;

    return st->current_scope->id.str;
}

bool symtable_set_current_scope(symtable_t *st, symtable_key_t id) {
    debug_entry();

    if (st == NULL)
        return false;

    for (symtable_scope_t *st_scope = st->first_scope; st_scope != NULL; st_scope = st_scope->next_scope) {
        if (strCmpConstStr(&st_scope->id, (char *) id) == 0) {
            st->current_scope = st_scope;
            return true;
        }
    }

    return false;
}

void symtable_set_first_scope(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return;

    st->current_scope = st->first_scope;
}

bool symtable_next_scope(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return false;

    if (st->current_scope->next_scope != NULL) {
        st->current_scope = st->current_scope->next_scope;
        return true;
    }

    return false;
}

symtable_t *symtable_new() {
    debug_entry();

    symtable_t *st = malloc(sizeof(struct symtable));
    if (st == NULL)
        return NULL;

    symtable_scope_t *st_scope = symtable_scope_new();
    if (st_scope == NULL)
        return NULL;

    st->num_of_scopes = 1;
    st->current_scope = st_scope;
    st->first_scope = st_scope;

    return st;
}

bool symtable_add_func_param(symtable_t *st, symtable_key_t id, token_type type) {
    debug_entry();

    if (st == NULL)
        return false;

    func_parameter_t *p = func_parameter_new();
    if (p == NULL)
        return false;

    for (size_t i = 0; i < strlen(id); i++)
        strAddChar(&p->id, id[i]);
    p->type = type;

    func_parameter_t *tmp_p = st->current_scope->first_parameter;

    if (tmp_p == NULL)
        st->current_scope->first_parameter = p;
    else {
        while (tmp_p->next_param != NULL)
            tmp_p = tmp_p->next_param;

        tmp_p->next_param = p;
    }

    st->current_scope->num_of_params++;
    debug("scope %s() - num of params: %lu",
            st->current_scope->id.str,
            st->current_scope->num_of_params);

    return true;
}

func_parameter_t *symtable_get_func_param(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return NULL;

    return st->current_scope->first_parameter;
}

int symtable_get_num_of_params(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return -1;

    return st->current_scope->num_of_params;
}

void symtable_add_func_return(symtable_t *st, token_type type) {
    debug_entry();

    if (st == NULL)
        return;

    func_return_t *r = func_return_new(type);

    func_return_t *tmp_r = st->current_scope->first_return;
    if (tmp_r == NULL)
        st->current_scope->first_return = r;
    else {
        while (tmp_r->next_return != NULL)
            tmp_r = tmp_r->next_return;

        tmp_r->next_return = r;
    }


    st->current_scope->num_of_ret_args++;
}

func_return_t *symtable_get_func_return(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return NULL;

    return st->current_scope->first_return;
}

int symtable_get_num_of_returns(symtable_t *st) {
    debug_entry();

    if (st == NULL)
        return -1;

    return st->current_scope->num_of_ret_args;
}


void symtable_free(symtable_t *st) {
    if (st == NULL)
        return;

    while (st->first_scope != NULL) {
        symtable_scope_t *next_scope = st->first_scope->next_scope;
        symtable_scope_free(st->first_scope);
        st->first_scope = next_scope;
        st->num_of_scopes--;
    }
    free(st);
}

symtable_iterator_t symtable_find(symtable_t *st, symtable_key_t key) {
    debug_entry();

    // Start in current row
    symtable_row_t *st_row = st->current_scope->st_stack->current_row;

    size_t i = symtable_hash_fun(key) % st_row->arr_size;

    for (; st_row != NULL; st_row = st_row->outer_row) {
        struct symtable_item *item = st_row->item_list[i];

        if (item == NULL)
            continue;

        symtable_iterator_t it = { item, st_row, i, 0 };

        while(symtable_iterator_valid(it)) {
            if (strcmp(it.ptr->key, key) == 0)
                return it;

            it = symtable_iterator_next(it);
        }
    }

    return symtable_row_end(st->current_scope->st_stack->current_row);
}

/**
 * @brief symtable_find_symbol looks for a symbol in symbol table matching a key
 *
 * @details key is used to search for an entry in a symtable. If key matches
 * an entry in a symtable, the returned pointer aims at the data. If not, the
 * returned pointer is NULL;
 *
 * @param t pointer to a symtable
 * @param key text
 *
 * @return pointer to symtable symbol
 *
 * @retval NULL searched symbol does not exist
 * @retval pointer search symbol does exist
 */
symtable_symbol_t *symtable_find_symbol(symtable_t *st, symtable_key_t key) {
    debug_entry();

    symtable_iterator_t it = symtable_find(st, key);
    if (symtable_iterator_valid(it))
        return &it.ptr->symbol;

    return NULL;
}

/**
 * @brief symtable_add_symbol adds new symbol into symbol table
 *
 * @param t pointer to an instance of symtable
 * @param key text
 *
 * @return symtable iterator
 *
 * @retval NULL error
 * @retval pointer newly created symbol
 */
symtable_symbol_t *symtable_add_symbol(symtable_t *st, symtable_key_t key) {
    debug_entry();

    if (st == NULL)
        return NULL;

    symtable_stack_t *st_stack = st->current_scope->st_stack;
    symtable_row_t *st_row = st_stack->current_row;

    size_t i = symtable_hash_fun(key) % st_row->arr_size;

    struct symtable_item *item = st_row->item_list[i];
    symtable_iterator_t it = { item, st_row, i, 0 };

    struct symtable_item *new_item = malloc(sizeof(struct symtable_item));
    if (new_item == NULL)
        return NULL;

    char *new_key = malloc(sizeof(char) * (strlen(key) + 1));
    if (new_key == NULL)
        return NULL;
    strcpy(new_key, key);

    new_item->key = new_key;
    new_item->next = NULL;

    // No magic is needed if the new item is in a bucket all alone
    if (st_row->item_list[it.idb] == NULL) {
        st_row->item_list[it.idb] = new_item;
        it.ptr = st_row->item_list[it.idb];
    } else {
        it.ptr = st_row->item_list[it.idb];
        if (strcmp(it.ptr->key, key) != 0) {
            while (it.ptr->next != NULL) {
                it.ptr = it.ptr->next;
                it.ide++;
            }

            // Realloc innner symtable if it gets too busy (a bucket is over 70% full)
            if ((it.ide / st_row->bucket_cap) >= 0.7f) {
                size_t max_bucket_count = st_row->arr_size;
                size_t new_max_bucket_count = max_bucket_count * 2;
                symtable_row_t *tmp_row = realloc(st_row, sizeof(struct symtable_row) + new_max_bucket_count * sizeof(struct symtable_item));
                if (tmp_row == NULL)
                    return NULL;

                st_stack->current_row = tmp_row;
                st_row = st_stack->current_row;

                for (size_t i = max_bucket_count; i < new_max_bucket_count; i++)
                    st_row->item_list[i] = NULL;

                st_row->arr_size = new_max_bucket_count;
                st_row->bucket_cap *= 2;

                // The symtable was reallocated so the last record in a bucket needs to
                // be found again.
                it.ptr = st_row->item_list[it.idb];
                while (it.ptr->next != NULL) {
                    it.ptr = it.ptr->next;
                    it.ide++;
                }
            }

            it.ptr->next = new_item;
            it.ptr = it.ptr->next;
        }
    }

    st_row->size++;

    return &it.ptr->symbol;
}

/**
 * @brief symtable_remove erases an item in a symbol table with a given key
 *
 * @param st pointer to an instance of symtable
 * @param key text
 */
void symtable_remove_symbol(symtable_t *st, symtable_key_t key) {
    debug_entry();
    if (st == NULL)
        return;

    symtable_iterator_t it = symtable_find(st, key);

    symtable_erase_symbol(it);
}

/**
 * @brief symtable_erase erases an item in a symbol table aimed at by an iterator
 *
 * @param st pointer to an instance of symtable
 * @param it symtable iterator
 */
void symtable_erase_symbol(symtable_iterator_t it) {
    debug_entry();

    symtable_iterator_t next_it = symtable_iterator_next(it);
    symtable_iterator_t prev_it = { it.st_row->item_list[it.idb], it.st_row, it.idb, 0 };

    // Iterator is at the beginning of a bucket
    if (symtable_iterator_equal(prev_it, it)) {
        if (symtable_iterator_valid(next_it) && it.idb == next_it.idb)
            it.st_row->item_list[it.idb] = next_it.ptr;
        else
            it.st_row->item_list[it.idb] = NULL;
    } else {
        // Find the entry right behind the erased one
        while (prev_it.ptr->next != it.ptr) {
            prev_it.ptr = prev_it.ptr->next;
        }

        if (symtable_iterator_valid(next_it) && it.idb == next_it.idb)
            prev_it.ptr->next = next_it.ptr;
        else
            prev_it.ptr->next = NULL;
    }

    free((char *) it.ptr->key);
    free(it.ptr);
}
