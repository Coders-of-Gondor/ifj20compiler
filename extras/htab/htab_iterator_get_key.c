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
 * @brief htab_iterator_get_key returns the text in a record
 *
 * @param t pointer to an instance of hashtable
 *
 * @return text
 */
htab_key_t htab_iterator_get_key(htab_iterator_t it) {
  return it.ptr->key;
}
