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

#include <stdlib.h>
#include "token.h"

struct symtable;
typedef struct symtable symtable_t;

typedef const char * symtable_key_t;
typedef token_t symtable_value_t;

struct symtable_item;

typedef struct symtable_iterator {
    struct symtable_item *ptr; /**< pointer to a record */
    const symtable_t *st; /**< pointer to a symtable on which the iterator operates */
    size_t idb; /**< id of a bucket where the iterator is */
    size_t ide; /**< id of an entry in a bucket */
} symtable_iterator_t;

size_t symtable_hash_fun(symtable_key_t key);

symtable_t *symtable_new();
void symtable_free(symtable_t *st);

size_t symtable_size(const symtable_t *st);
size_t symtable_bucket_count(const symtable_t *st);
size_t symtable_bucket_cap(const symtable_t *st);

symtable_iterator_t symtable_find(symtable_t *st, symtable_key_t key);
symtable_iterator_t symtable_lookup_add(symtable_t *st, symtable_key_t key);

void symtable_clear(symtable_t *st);
void symtable_remove(symtable_t *st, symtable_key_t key);
void symtable_erase(symtable_t *st, symtable_iterator_t it);

symtable_iterator_t symtable_begin(const symtable_t *st);
symtable_iterator_t symtable_end(const symtable_t *st);
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it);

symtable_value_t symtable_iterator_get_value(symtable_iterator_t it);
symtable_value_t symtable_iterator_set_value(symtable_iterator_t it, symtable_value_t val);

static inline int symtable_iterator_valid(symtable_iterator_t it) {
  return it.ptr != NULL;
}
static inline int symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2) {
  return it1.ptr == it2.ptr && it1.st == it2.st;
}

#endif // __SYMTABLE_H__
