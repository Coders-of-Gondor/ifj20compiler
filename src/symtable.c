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

#include <stdio.h>
#include <stdlib.h>
#include "symtable-private.h"
#include "token.h"
#include "../extras/htab/htab.h"

#define STM_INIT_STACK_MAX 4

/**
 * @brief symtable_manager_new initializes an instance symtable manager
 *
 * @details symtable_manager_new allocates space for a set number of stacks. If
 * more space is needed, symtable_manager_push_stack will reallocate the
 * manager. Manager starts with 1 stack. Initial maximum number of stacks is 4.
 *
 * @return an instance of symtable manager (or NULL on error)
 */
symtable_manager_t *symtable_manager_new() {
  symtable_manager_t *stm = malloc(sizeof(struct symtable_manager) + STM_INIT_STACK_MAX * sizeof(struct symtable));
  if (stm == NULL)
    return NULL;

  stm->stack_count = 1;
  stm->max_stack_count = STM_INIT_STACK_MAX;

  symtable_t *st = symtable_new();
  if (st == NULL)
    return NULL;

  for (size_t i = 0; i < stm->max_stack_count; i++)
    stm->symtables[i] = NULL;

  stm->symtables[0] = st;

  return stm;
}

/**
 * @brief symtable_manager_free destroys an instance of symtable manager
 *
 * @param stm an instance of symtable manager
 */
void symtable_manager_free(symtable_manager_t *stm) {
  if (stm != NULL) {
    for (size_t i = 0; i < stm->stack_count; i++)
      symtable_free(stm->symtables[i]);
    free(stm);
  }
}

/**
 * @brief symtable_manager_push_stack adds a new stack of symtables
 *
 * @details symtable_manager_push_stack checks before adding a stack if there's
 * enough space for it. If there is not, it reallocates the manager to
 * accomodate twice as many symtable stacks.
 *
 * @param stm an instance of symtable manager
 */
void symtable_manager_push_stack(symtable_manager_t *stm) {
  if (stm == NULL)
    return;

  symtable_t *st = symtable_new();
  if (st == NULL)
    return;

  if (stm->stack_count == stm->max_stack_count) {
    symtable_manager_t *tmp_stm = realloc(stm, sizeof(struct symtable_manager) + stm->max_stack_count * 2 * sizeof(struct symtable));
    if (tmp_stm == NULL)
      return;

    stm = tmp_stm;
    stm->max_stack_count *= 2;
  }

  stm->symtables[stm->stack_count] = st;
  stm->stack_count++;
}

/**
 * @brief symtable_manager_pop_stack removes the topmost stack of symtables
 *
 * @details symtable_manager_pop_stack pops a stack only if there's more than
 * one stack in the manager
 *
 * @param stm an instance of symtable manager
 */
void symtable_manager_pop_stack(symtable_manager_t *stm) {
  if (stm == NULL)
    return;

  if (stm->stack_count > 1) {
    symtable_free(stm->symtables[stm->stack_count]);
    stm->stack_count--;
  }
}

/**
 * @brief symtable_manager_add_symbol adds a symbol to the topmost symtable stack
 *
 * @details symtable_manager_add_symbol first
 *
 * @param stm an instance of symtable manager
 * @param token a pointer to token
 *
 * @return success code
 *
 * @retval 0 success
 * @retval 1 symbol redefinition
 * @retval 2 invalid instance of symtable manager
 */
int symtable_manager_add_symbol(symtable_manager_t *stm, token_t *token) {

  if (stm == NULL)
    return 2;

  for (int i = stm->stack_count; i > 0; i--) {
    token_t *tmp_token = symtable_find_symbol(stm->symtables[stm->stack_count], "asd");
    if (tmp_token != NULL)
      return 1;
  }

  symtable_add_symbol(stm->symtables[stm->stack_count-1], token);

  return 0;
}

/**
 * @brief symtable_manager_remove_symbol removes a symbol
 *
 * @param stm an instane of symtable manager
 * @param token pointer to token
 */
void symtable_manager_remove_symbol(symtable_manager_t *stm, token_t *token) {
  if (stm == NULL)
    return;

  symtable_t *st = stm->symtables[stm->stack_count];

  symtable_remove_symbol (st, token);
}

/**
 * @brief symtable_manager_find_symbol looks for a symbol in a stack of symtables and returns it
 *
 * @details symtable_manager_find_symbol traverses the stacks from top to bottom
 * while looking for a symbol. It either returns the symbol or NULL.
 *
 * @param stm an instance of symtable manager
 * @param key symbol identificator
 * @param type type of the symbol
 */
token_t *symtable_manager_find_symbol(symtable_manager_t *stm, htab_key_t key, token_type type) {
  if (stm == NULL)
    return NULL;

  token_t *token;
  for (int i = stm->stack_count; i > 0; i--) {
    token = symtable_find_symbol(stm->symtables[stm->stack_count], key, type);
  }

  return token;
}

/**
 * @brief symtable_manager_has_symbol checks if a symbol is in a stack of symtables
 *
 * @details symtable_manager_find_symbol traverses the stacks from top to bottom
 * while looking for a symbol. It reports if the stack contains the sybol or
 * not.
 *
 * @param stm an instance of symtable manager
 * @param token pointer to a token
 */
int symtable_manager_has_symbol(symtable_manager_t *stm, token_t *token) {
  if (stm == NULL)
    return 1;

  for (int i = stm->stack_count; i > 0; i--) {
    token_t *token = symtable_find_symbol(stm->symtables[stm->stack_count], key, type);
    if (token != NULL)
      return 0;
  }

  return 2;
}

/**
 * @brief symtable_new initializes an instance of symtable
 *
 * @param type type of tokens to be stored in a table of symbols
 *
 * @return an instance of symtable (or NULL on error)
 */
symtable_t *symtable_new() {
  htab_t *hash_tables[3];

  // TODO: each hash table requires a different deconstructor and printf is definitely not it :)
  for (int i = 0; i < 3; i++) {
    hash_tables[i] = htab_init(printf);
    if (hash_tables[i] == NULL)
      return NULL;
  }

  symtable_t *st = malloc(sizeof(struct symtable));
  if (st == NULL)
    return NULL;

  st->ht_variables = hash_tables[0];
  st->ht_constants = hash_tables[1];
  st->ht_functions = hash_tables[2];

  return st;
}

/**
 * @brief symtable_free destroys an instance of symtable
 *
 * @param s an instance of symtable
 */
void symtable_free(symtable_t *st) {
  if (st != NULL) {
    htab_free(st->ht_variables);
    htab_free(st->ht_constants);
    htab_free(st->ht_functions);

    free(st);
  }
}

void symtable_add_symbol(symtable_t *st, token_t *token) {
  if (st == NULL)
    return;
}

void symtable_remove_symbol(symtable_t *st, token_t *token) {
  if (st == NULL)
    return;
}

token_t *symtable_find_symbol(symtable_t *st, htab_key_t key) {
  if (st == NULL)
    return NULL;
}
