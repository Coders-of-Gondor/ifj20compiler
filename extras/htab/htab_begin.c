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
#include <stdio.h>

/**
 * @brief htab_begin returns a new iterator aiming at the first object in a hashtable
 *
 * @details htab_begins looks for the first object in a hashtable. If it's not
 * found, it returns htab_end(t).
 *
 * @param t pointer to an instance of hashtable
 *
 * @return hashtable iterator
 */
htab_iterator_t htab_begin(const htab_t *t) {
  htab_iterator_t it = { t->table->item_list[0], t, 0, 0 };

  while(!htab_iterator_valid(it) && it.idb < htab_bucket_count(t)) {
    it.ptr = t->table->item_list[it.idb];
    it.idb++;
  }

  if (!htab_iterator_valid(it))
    it = htab_end(t);

  return it;
}
