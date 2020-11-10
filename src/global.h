/**
 * @file global.h 
 * @author Marek Filip <xfilip46, Wecros>
 * @author Ondřej Míchal <xmicha80>
 * FIT BUT 
 * @date 10/11/2020
 * @brief Header file for global variable definitions.
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "symtable.h"

/**
 * @brief Decleration of a global variable tracking symbol table holding keywords.
 */
extern symtable_t *keywords_symtable;

/**
 * Global tracker of the lookahead token.
 */
extern token_t lookahead;

/**
 * Global tracker of the scanner struct.
 */
extern scanner_t scanner;

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
