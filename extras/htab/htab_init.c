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
 * @brief htab_init constructs a new hashtable
 *
 * @param n number of "buckets"
 *
 * @return pointer to a hashtable
 */
htab_t *htab_init(size_t n) {
  htab_t *hash_table = malloc(sizeof(struct htab) + (n * sizeof(struct htab_item)));
  if (hash_table == NULL)
    return NULL;

  hash_table->size = 0;
  hash_table->arr_size = n;

  for (size_t i = 0; i < hash_table->arr_size; i++) {
    hash_table->item_list[i] = NULL;
  }

  return hash_table;
}
