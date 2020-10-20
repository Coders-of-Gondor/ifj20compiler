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

#define INIT_NUM_OF_BUCKETS 16
#define INIT_NUM_IN_BUCKET 16

/**
 * @brief htab_init constructs a new hashtable
 *
 * @details an instance of hashtable is capable of storing only a single type
 * of values. This is done by passing a value_deconstructor function that takes
 * care of freeing of the value.
 *
 * @param value_deconstructor function used to free values in entries
 *
 * @return pointer to a hashtable
 */
htab_t *htab_init(void (*value_deconstructor) (htab_value_t value)) {
  htab_t *hash_table = malloc(sizeof(struct htab));
  if (hash_table == NULL)
    return NULL;

  hash_table->value_deconstructor = value_deconstructor;

  hash_table->table = malloc(sizeof(struct htab_table) + INIT_NUM_OF_BUCKETS * sizeof(void *));
  if (hash_table->table == NULL)
    return NULL;

  hash_table->table->size = 0;
  hash_table->table->arr_size = INIT_NUM_OF_BUCKETS;
  hash_table->table->bucket_cap = INIT_NUM_IN_BUCKET;

  for (size_t i = 0; i < hash_table->table->arr_size; i++) {
    hash_table->table->item_list[i] = NULL;
  }

  return hash_table;
}
