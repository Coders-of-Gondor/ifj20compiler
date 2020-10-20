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
 * @brief htab_iterator_next returns an iterator pointing to the next record
 *
 * @param it hashtable iterator
 *
 * @return hashtable iterator
 */
htab_iterator_t htab_iterator_next(htab_iterator_t it) {
  if (it.ptr != NULL) {
    if (it.ptr->next != NULL) {
      it.ptr = it.ptr->next;
      it.ide++;
      return it;
    }
  }

  for (++it.idb; it.idb < htab_bucket_count(it.t); it.idb++) {
    if (it.t->table->item_list[it.idb] != NULL) {
      it.ptr = it.t->table->item_list[it.idb];
      it.ide = 0;

      return it;
    }
  }
  return htab_end(it.t);

}
