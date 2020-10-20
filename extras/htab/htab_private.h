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

#include "htab.h"

struct htab_item {
  htab_key_t key;
  htab_value_t data;
  struct htab_item *next;
  struct htab_item *prev;
};

struct htab_table {
  size_t size;
  size_t arr_size;
  size_t bucket_cap;
  void *item_list[];
};

struct htab {
  void (*value_deconstructor) (htab_value_t value);
  struct htab_table *table;
};

