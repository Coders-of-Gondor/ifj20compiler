/* global.h
 * Ondřej Míchal <xmicha80>
 * FIT BUT
 * 11/11/2020
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "symtable.h"

/**
 * @brief Decleration of a global variable tracking symbol table holding keywords.
 */
extern symtable_t *keywords_symtable;

struct keyword {
  char lit[20];
  token_type type;
};

/**
 * @brief Global initialization function for various things.
 * @description Set up the symbol table holding keywords.
 *
 * @retval 0: Succesful initialization.
 * @retval 1: Failed to initialize.
 */
int global_init();

#endif // __GLOBAL_H__
