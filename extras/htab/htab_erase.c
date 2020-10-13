/*
 * Copyright (C) 2019-2020 Ondřej Míchal

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
#include "htab_private.h"
#include <stdlib.h>

/**
 * @brief htab_erase erases an item in a hashtable aimed at by an iterator
 *
 * @param t pointer to an instance of hashtable
 * @param it hashtable iterator
 */
void htab_erase(htab_t *t, htab_iterator_t it) {
  if (!htab_iterator_valid(it))
    return;

  htab_iterator_t next_it = htab_iterator_next(it);
  htab_iterator_t prev_it = {
    t->item_list[it.idx],
    t,
    it.idx
  };

  free((char *) it.ptr->key);
  free(it.ptr);

  // Iterator is at the beginning of a bucket
  if (htab_iterator_equal(prev_it, it)) {
    if (htab_iterator_valid(next_it) && it.idx == next_it.idx)
      t->item_list[it.idx] = next_it.ptr;
    else
      t->item_list[it.idx] = NULL;
  } else {
    // Find the entry right behind the erased one
    while (prev_it.ptr->next != it.ptr) {
      prev_it.ptr = prev_it.ptr->next;
    }

    if (htab_iterator_valid(next_it) && it.idx == next_it.idx)
      prev_it.ptr->next = next_it.ptr;
    else
      prev_it.ptr->next = NULL;
  }
}
