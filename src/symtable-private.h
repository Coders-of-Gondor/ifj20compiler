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

#include "symtable.h"

struct symtable_item {
  symtable_key_t key;
  symtable_value_t data;
  struct symtable_item *next;
};

struct symtable_body {
  size_t size;
  size_t arr_size;
  size_t bucket_cap;
  struct symtable_item *item_list[];
};

struct symtable {
  struct symtable_body *stb;
};

struct symtable_stack_body {
  size_t stack_count;
  size_t max_stack_count;

  symtable_t *symtables[];
};

struct symtable_stack {
  struct symtable_stack_body *stsb;
};

