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
#include <stdint.h>

/**
 * @brief htab_hash_fun returns a hash for a hashtable key
 *
 * @param str text
 *
 * @return hash
 */
size_t htab_hash_fun(htab_key_t str) {
  // Must be 32 bits
  uint32_t h = 0;
  const unsigned char *p;

  for (p = (const unsigned char*) str; *p != '\0'; p++) {
    h = 65599 * h + *p;
  }

  return h;
}
