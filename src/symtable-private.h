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

#include "symtable.h"
#include "../extras/htab/htab.h"

struct symtable_manager {
  size_t stack_count;
  size_t max_stack_count;

  symtable_t *symtables[];
};

struct symtable {
  htab_t *ht_variables;
  htab_t *ht_constants;
  htab_t *ht_functions;
};

void generate_key_from_token(token_t *token, htab_key_t *key);
