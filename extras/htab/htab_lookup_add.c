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
#include <string.h>

/**
 * @brief htab_lookup_add looks for a key in a hashtable and adds new if not found
 *
 * @param t pointer to an instance of hashtable
 * @param key text
 *
 * @return hashtable iterator
 */
htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key) {
  htab_iterator_t it = htab_find(t, key);
  if (htab_iterator_valid(it))
    return it;

  struct htab_item *new_item = malloc(sizeof(struct htab_item));
  if (new_item == NULL)
    return htab_end(t);

  char *new_key = malloc(sizeof(char) * (strlen(key) + 1));
  if (new_key == NULL)
    return htab_end(t);
  strcpy(new_key, key);

  new_item->key = new_key;
  new_item->data = 0;
  new_item->next = NULL;

  size_t i = htab_hash_fun(key) % htab_bucket_count(t);
  if (t->item_list[i] == NULL)
    t->item_list[i] = new_item;

  it.ptr = t->item_list[i];
  if (strcmp(it.ptr->key, key) != 0) {
    while (it.ptr->next != NULL) {
      it.ptr = it.ptr->next;
    }

    it.ptr->next = new_item;
    it.ptr = it.ptr->next;
  }

  it.idx = i;
  t->size++;

  return it;
}
