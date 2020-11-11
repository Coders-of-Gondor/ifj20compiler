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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "symtable-private.h"
#include "token.h"

#define ST_INIT_NUM_OF_BUCKETS 65536
#define ST_INIT_NUM_IN_BUCKET 16
#define STM_INIT_STACK_MAX 256

/**
 * @brief symtable_hash_fun returns a hash for a hashtable key
 *
 * @param key text
 *
 * @return hash
 */
size_t symtable_hash_fun(symtable_key_t key) {
  uint32_t h = 0; // Must be 32 bits
  const unsigned char *p;

  for (p = (const unsigned char*) key; *p != '\0'; p++) {
    h = 65599 * h + *p;
  }

  return h;
}

/**
 * @brief symtable_new constructs a new symbol table
 *
 * @return pointer to an instance of symtable
 *
 * @retval NULL error
 * @retval pointer success
 */
symtable_t *symtable_new() {
  debug_entry();
  symtable_t *st = malloc(sizeof(struct symtable));
  if (st == NULL)
    return NULL;

  st->stb = malloc(sizeof(struct symtable_body) + ST_INIT_NUM_OF_BUCKETS * sizeof(struct symtable_item *));
  if (st->stb == NULL)
    return NULL;

  st->stb->size = 0;
  st->stb->arr_size = ST_INIT_NUM_OF_BUCKETS;
  st->stb->bucket_cap = ST_INIT_NUM_IN_BUCKET;

  for (size_t i = 0; i < st->stb->arr_size; i++)
    st->stb->item_list[i] = NULL;

  return st;
}

/**
 * @brief symtable_free clears and destroys an instance symtable
 *
 * @param st pointer to an instance of symtable
 */
void symtable_free(symtable_t *st) {
  debug_entry();
  if (st != NULL) {
    symtable_clear(st);
    free(st->stb);
    free(st);
  }
}

/**
 * @brief symtable_size returns the number of records in a symbol table
 *
 * @param t pointer to an instance of symtable
 *
 * @return number of records
 */
size_t symtable_size(const symtable_t *st) {
  return st->stb->size;
}

/**
 * @brief symtable_bucket_count returns the number of "buckets" in a symbol table
 *
 * @param t pointer to an instance of symtable
 *
 * @return number of "buckets" in a symtable
 */
size_t symtable_bucket_count(const symtable_t *st) {
  return st->stb->arr_size;
}

/**
 * @brief symtable_bucket_cap returns the number of "buckets" in a symbol table
 *
 * @param t pointer to an instance of symtable
 *
 * @return number of "buckets" in a symtable
 */
size_t symtable_bucket_cap(const symtable_t *st) {
  return st->stb->bucket_cap;
}

/**
 * @brief symtable_find looks for a record in symbol table matching a key
 *
 * @details key is used to search for an entry in a symtable. If key matches
 * an entry in a symtable, the returned iterator aims at it. If not, the
 * returned iterator is symtable_end(t);
 *
 * @param t pointer to an instance of symtable
 * @param key text
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_find(symtable_t *st, symtable_key_t key) {
  debug_entry();
  size_t i = symtable_hash_fun(key) % symtable_bucket_count(st);
  struct symtable_item *item = st->stb->item_list[i];

  if (item == NULL)
    return symtable_end(st);

  symtable_iterator_t it = { item, st, i, 0 };

  while(symtable_iterator_valid(it)) {
    if (strcmp(it.ptr->key, key) == 0) {
      break;
    }

    if (item->next == NULL) {
      return symtable_end(st);
    }

    it = symtable_iterator_next(it);
  }

  return it;
}

/**
 * @brief symtable_lookup_add looks for a key in a symbol table and adds new if not found
 *
 * @param t pointer to an instance of symtable
 * @param key text
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_lookup_add(symtable_t *st, symtable_key_t key) {
  debug_entry();
  symtable_iterator_t it = symtable_find(st, key);
  if (symtable_iterator_valid(it))
    return it;

  size_t i = symtable_hash_fun(key) % symtable_bucket_count(st);
  it.idb = i;
  it.ide = 0; // Iterator starts at the beginning of a bucket

  struct symtable_item *new_item = malloc(sizeof(struct symtable_item));
  if (new_item == NULL)
    return symtable_end(st);

  char *new_key = malloc(sizeof(char) * (strlen(key) + 1));
  if (new_key == NULL)
    return symtable_end(st);
  strcpy(new_key, key);

  new_item->key = new_key;
  new_item->next = NULL;

  // No magic is needed if the new item is in a bucket all alone
  if (st->stb->item_list[it.idb] == NULL) {
    st->stb->item_list[it.idb] = new_item;
    it.ptr = st->stb->item_list[it.idb];
  } else {
    it.ptr = st->stb->item_list[it.idb];
    if (strcmp(it.ptr->key, key) != 0) {
      while (it.ptr->next != NULL) {
        it.ptr = it.ptr->next;
        it.ide++;
      }

      // Realloc innner symtable if it gets too busy (a bucket is over 70% full)
      if ((it.ide / symtable_bucket_cap(st)) >= 0.7f) {
        size_t max_bucket_count = symtable_bucket_count(st);
        size_t new_max_bucket_count = max_bucket_count * 2;
        struct symtable_body *tmp_stb = realloc(st->stb, sizeof(struct symtable_body) + new_max_bucket_count * sizeof(struct symtable_item));
        if (tmp_stb == NULL)
          return symtable_end(st);

        st->stb = tmp_stb;

        for (size_t i = max_bucket_count; i < new_max_bucket_count; i++)
          st->stb->item_list[i] = NULL;

        st->stb->arr_size = new_max_bucket_count;
        st->stb->bucket_cap *= 2;

        // The symtable was reallocated so the last record in a bucket needs to
        // be found again.
        it.ptr = st->stb->item_list[it.idb];
        while (it.ptr->next != NULL) {
          it.ptr = it.ptr->next;
          it.ide++;
        }
      }

      it.ptr->next = new_item;
      it.ptr = it.ptr->next;
    }
  }

  st->stb->size++;

  return it;
}

/**
 * @brief symtable_clear erases all items in a symbol table
 *
 * @param t pointer to an instance of symtable
 */
void symtable_clear(symtable_t *st) {
  debug_entry();
  if (st == NULL)
    return;

  symtable_iterator_t it = symtable_begin(st);
  symtable_iterator_t next_it;

  while (symtable_iterator_valid(it)) {
    next_it = symtable_iterator_next(it);
    symtable_erase(st, it);
    it = next_it;
  }
}

/**
 * @brief symtable_remove erases an item in a symbol table with a given key
 *
 * @param st pointer to an instance of symtable
 * @param key text
 */
void symtable_remove(symtable_t *st, symtable_key_t key) {
  debug_entry();
  if (st == NULL)
    return;

  symtable_iterator_t it = symtable_find(st, key);

  symtable_erase(st, it);
}

/**
 * @brief symtable_erase erases an item in a symbol table aimed at by an iterator
 *
 * @param st pointer to an instance of symtable
 * @param it symtable iterator
 */
void symtable_erase(symtable_t *st, symtable_iterator_t it) {
  debug_entry();
  if (st == NULL)
    return;

  symtable_iterator_t next_it = symtable_iterator_next(it);
  symtable_iterator_t prev_it = { st->stb->item_list[it.idb], st, it.idb, 0 };

  // Iterator is at the beginning of a bucket
  if (symtable_iterator_equal(prev_it, it)) {
    if (symtable_iterator_valid(next_it) && it.idb == next_it.idb)
      st->stb->item_list[it.idb] = next_it.ptr;
    else
      st->stb->item_list[it.idb] = NULL;
  } else {
    // Find the entry right behind the erased one
    while (prev_it.ptr->next != it.ptr) {
      prev_it.ptr = prev_it.ptr->next;
    }

    if (symtable_iterator_valid(next_it) && it.idb == next_it.idb)
      prev_it.ptr->next = next_it.ptr;
    else
      prev_it.ptr->next = NULL;
  }

  free((char *) it.ptr->key);
  free(it.ptr);
}

/**
 * @brief symtable_begin returns a new iterator aiming at the first object in a symbol table
 *
 * @details symtable_begins looks for the first object in a symtable. If it's
 * not found, it returns symtable_end(t).
 *
 * @param t pointer to an instance of symtable
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_begin(const symtable_t *st) {
  symtable_iterator_t it = { st->stb->item_list[0], st, 0, 0 };

  while(it.idb < symtable_bucket_count(st)) {
    it.ptr = st->stb->item_list[it.idb];

    if (symtable_iterator_valid(it))
      return it;

    it.idb++;
  }

  return symtable_end(st);
}

symtable_iterator_t symtable_end(const symtable_t *st) {
  symtable_iterator_t it = { NULL, st, symtable_bucket_count(st), symtable_bucket_cap(st) };

  return it;
}

/**
 * @brief symtable_iterator_next returns an iterator pointing to the next record
 *
 * @param it symtable iterator
 *
 * @return symtable iterator
 */
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it) {
  if (it.ptr != NULL) {
    if (it.ptr->next != NULL) {
      it.ptr = it.ptr->next;
      it.ide++;
      return it;
    }
  }

  for (++it.idb; it.idb < symtable_bucket_count(it.st); it.idb++) {
    if (it.st->stb->item_list[it.idb] != NULL) {
      it.ptr = it.st->stb->item_list[it.idb];
      it.ide = 0;

      return it;
    }
  }

  return symtable_end(it.st);
}

/**
 * @brief symtable_iterator_get_value gets the value of a record
 *
 * @param it hashtable iterator
 *
 * @return record value (token)
 */
symtable_value_t symtable_iterator_get_value(symtable_iterator_t it) {
  return it.ptr->data;
}


/**
 * @brief symtable_iterator_set_value sets the value (token) of a record
 *
 * @details by giving a record a value (token), symtable "takes over ownership"
 * and will take care of deallocating the token when a record is deleted or the
 * whole symtable is destroyed.
 *
 * @param it hashtable iterator
 * @param val token
 *
 * @return record value (token)
 */
symtable_value_t symtable_iterator_set_value(symtable_iterator_t it, symtable_value_t val) {
  it.ptr->data = val;

  return it.ptr->data;
}

/**
 * @brief symtable_manager_new initializes an instance symtable manager
 *
 * @details symtable_manager_new allocates space for a set number of stacks. If
 * more space is needed, symtable_manager_push_stack will reallocate the
 * manager. Manager starts with 1 stack. Initial maximum number of stacks is 4.
 *
 * @return pointer to an instance of symtable manager
 *
 * @retval NULL error
 * @retval pointer success
 */
symtable_manager_t *symtable_manager_new() {
  debug_entry();
  symtable_manager_t *stm = malloc(sizeof(struct symtable_manager));
  if (stm == NULL)
    return NULL;

  stm->stmb = malloc(sizeof(struct symtable_manager_body) + STM_INIT_STACK_MAX * sizeof(struct symtable));
  if (stm->stmb == NULL)
    return NULL;

  stm->stmb->stack_count = 1;
  stm->stmb->max_stack_count = STM_INIT_STACK_MAX;

  symtable_t *st = symtable_new();
  if (st == NULL)
    return NULL;

  for (size_t i = 0; i < symtable_manager_max_stack_size(stm); i++)
    stm->stmb->symtables[i] = NULL;

  stm->stmb->symtables[0] = st;

  return stm;
}

/**
 * @brief symtable_manager_free destroys an instance of symtable manager
 *
 * @param stm pointer to an instance of symtable manager
 */
void symtable_manager_free(symtable_manager_t *stm) {
  debug_entry();
  if (stm != NULL) {
    for (size_t i = 0; i < stm->stmb->stack_count; i++)
      symtable_free(stm->stmb->symtables[i]);
    free(stm->stmb);
    free(stm);
  }
}

size_t symtable_manager_stack_size(const symtable_manager_t *stm) {
  return stm->stmb->stack_count;
}

size_t symtable_manager_max_stack_size(const symtable_manager_t *stm) {
  return stm->stmb->max_stack_count;
}

/**
 * @brief symtable_manager_push adds a new symtable to its stack
 *
 * @details symtable_manager_push checks before adding a stack if there's
 * enough space for it. If there is not, it reallocates the manager to
 * accomodate twice as many symtable stacks.
 *
 * @param stm pointer to an instance of symtable manager
 */
void symtable_manager_push(symtable_manager_t *stm) {
  debug_entry();
  if (stm == NULL)
    return;

  symtable_t *st = symtable_new();
  if (st == NULL)
    return;

  if (stm->stmb->stack_count == stm->stmb->max_stack_count) {
    size_t max_stack_size = symtable_manager_max_stack_size(stm);
    size_t new_max_stack_size = max_stack_size * 2;
    struct symtable_manager_body *tmp_stmb = realloc(stm->stmb, sizeof(struct symtable_manager_body) + new_max_stack_size * sizeof(struct symtable));
    if (tmp_stmb == NULL)
      return;

    stm->stmb = tmp_stmb;

    for (size_t i = max_stack_size; i < new_max_stack_size; i++)
      stm->stmb->symtables[i] = NULL;

    stm->stmb->max_stack_count = new_max_stack_size;
  }

  stm->stmb->symtables[symtable_manager_stack_size(stm)] = st;
  stm->stmb->stack_count++;
}

/**
 * @brief symtable_manager_pop removes the topmost symtable in its stack
 *
 * @details symtable_manager_pop pops a stack only if there's more than
 * one stack in the manager. The popped symtable is not kept in memory.
 *
 * @param stm pointer to an instance of symtable manager
 */
void symtable_manager_pop(symtable_manager_t *stm) {
  debug_entry();
  if (stm == NULL)
    return;

  if (stm->stmb->stack_count > 1) {
    symtable_t *st = symtable_manager_get_top(stm);
    symtable_free(st);
    stm->stmb->stack_count--;
  }
}

/**
 * @brief symtable_manager_get_top returns the topmost symtable in the stack
 *
 * @return pointer to an instance of symtable
 *
 * @retval NULL error
 * @retval pointer success
 */
symtable_t *symtable_manager_get_top(symtable_manager_t *stm) {
  if (stm == NULL)
    return NULL;

  return stm->stmb->symtables[symtable_manager_stack_size(stm) - 1];
}

/**
 * @brief symtable_manager_find looks for a key in a stack of symtables
 *
 * @details symtable_manager_find_symbol traverses the stacks from top to bottom
 * while looking for a key.
 *
 * @param stm pointer to an instance of symtable manager
 * @param key symbol identificator
 *
 * @return symtable_iterator
 *
 * @retval symtable_end(st) searched key does not exist
 * @retval symtable_iterator iterator aiming at the key
 */
symtable_iterator_t symtable_manager_find(symtable_manager_t *stm, symtable_key_t key) {
  debug_entry();
  symtable_t *st = symtable_manager_get_top(stm);
  symtable_iterator_t it = { NULL, st, symtable_bucket_count(st), symtable_bucket_cap(st) };

  for (int i = stm->stmb->stack_count - 1; i > 0 && !symtable_iterator_valid(it); i--) {
    it = symtable_find(stm->stmb->symtables[i], key);
  }

  return it;
}

/**
 * @brief symtable_manager_lookup_add adds a symbol to the topmost symtable
 *
 * @details symtable_manager_add_symbol firstly searches for a key in all stacks
 * of symtables. If a key is found, an iterator aiming at the entry is returned.
 * If a key is not found, it is added to the topmost symtable and iterator
 * aiming at it is returned.
 *
 * @param stm pointer to an instance of symtable manager
 * @param key symbol identificator
 *
 * @return symtable_iterator
 */
symtable_iterator_t symtable_manager_lookup_add(symtable_manager_t *stm, symtable_key_t key) {
  debug_entry();
  symtable_t *st = symtable_manager_get_top(stm);
  symtable_iterator_t it = { NULL, st, symtable_bucket_count(st), symtable_bucket_cap(st) };

  it = symtable_manager_find(stm, key);
  if (symtable_iterator_valid(it))
    return it;

  it = symtable_lookup_add(st, key);

  return it;
}

/**
 * @brief symtable_manager_has checks if a symbol is in a stack of symtables
 *
 * @details symtable_manager_has traverses the stacks from top to bottom while
 * looking for a symbol. It reports if the stack contains the symbol or not.
 *
 * @param stm pointer to an instance of symtable manager
 * @param key symbol identificator
 */
int symtable_manager_has(symtable_manager_t *stm, symtable_key_t key) {
  symtable_iterator_t it = symtable_manager_find(stm, key);
  if (!symtable_iterator_valid(it))
    return 1;

  return 0;
}

/**
 * @brief symtable_manager_remove removes a symbol from the topmost symtable
 *
 * @param stm pointer to an instane of symtable manager
 * @param key symbol identificator
 */
void symtable_manager_remove(symtable_manager_t *stm, symtable_key_t key) {
  if (stm == NULL)
    return;

  symtable_t *st = symtable_manager_get_top(stm);

  symtable_remove(st, key);
}
