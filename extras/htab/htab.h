/*
 * Copyright (C) 2019-2020 Ondřej Míchal, Petr Peringer

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

#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>
#include <stdbool.h>

struct htab;
typedef struct htab htab_t;

typedef const char * htab_key_t;
typedef int htab_value_t;

struct htab_item;

typedef struct htab_iterator {
    struct htab_item *ptr; /**< pointer to a record */
    const htab_t *t; /**< pointer to a hashtable on which the iterator operates */
    size_t idx; /**< id of a bucket where the iterator is */
} htab_iterator_t;

size_t htab_hash_fun(htab_key_t str);

htab_t *htab_init(size_t n);
void htab_free(htab_t * t);

size_t htab_size(const htab_t * t);
size_t htab_bucket_count(const htab_t * t);

htab_iterator_t htab_find(htab_t * t, htab_key_t key);
htab_iterator_t htab_lookup_add(htab_t * t, htab_key_t key);

void htab_erase(htab_t * t, htab_iterator_t it);
void htab_clear(htab_t * t);

htab_iterator_t htab_begin(const htab_t * t);
htab_iterator_t htab_end(const htab_t * t);
htab_iterator_t htab_iterator_next(htab_iterator_t it);

htab_key_t htab_iterator_get_key(htab_iterator_t it);
htab_value_t htab_iterator_get_value(htab_iterator_t it);
htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val);

static inline bool htab_iterator_valid(htab_iterator_t it) {
  return it.ptr != NULL;
}
static inline bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
  return it1.ptr == it2.ptr && it1.t == it2.t;
}

#endif // __HTAB_H__
