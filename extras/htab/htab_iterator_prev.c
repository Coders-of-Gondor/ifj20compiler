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
 * @brief htab_iterator_prev returns an iterator pointing to a previous record
 *
 * @param it hashtable iterator
 *
 * @return hashtable iterator
 */
htab_iterator_t htab_iterator_prev(htab_iterator_t it) {
  if (it.ptr != NULL) {
    if (it.ptr->prev != NULL) {
      it.ptr = it.ptr->prev;
      it.ide--;
      return it;
    }
  }

  for (--it.idb /* start in previous bucket */; ; it.idb--) {
    if (it.t->table->item_list[it.idb] != NULL) {
      it.t = it.t->table->item_list[it.idb];
      it.ide = 0;

      while (it.ptr->next != NULL) {
        it.ptr = it.ptr->next;
        it.ide++;
      }

      return it;
    }

    if (it.idb == 0)
      break;
  }

  return htab_end(it.t);
}
