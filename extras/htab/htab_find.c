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

/**
 * @brief htab_find looks for a record in hashtable matching a key
 *
 * @details key is used to search for an entry in a hashtable. If key matches
 * an entry in a hashtable, the returned iterator aims at it. If not, the
 * returned iterator is htab_end(t);
 *
 * @param t pointer to an instance of hashtable
 * @param key text
 *
 * @return hashtable iterator
 */
htab_iterator_t htab_find(htab_t *t, htab_key_t key) {
  size_t i = htab_hash_fun(key) % htab_bucket_count(t);
  struct htab_item *item = t->item_list[i];

  if (item == NULL)
    return htab_end(t);

  while(item != NULL) {
    if (strcmp(item->key, key) == 0) {
      break;
    }

    if (item->next == NULL) {
      return htab_end(t);
    }

    item = item->next;
  }

  htab_iterator_t it = {item, t, i};

  if (!htab_iterator_valid(it))
    return htab_end(t);

  return it;
}
