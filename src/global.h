/**
 * @file global.h 
 * @author Marek Filip <xfilip46>
 * @author Ondřej Míchal <xmicha80>
 * @brief Header file for global variable definitions.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "three-address-code.h"

/**
 * @brief Decleration of a global variable tracking symbol table holding keywords.
 */
extern symtable_t *keywords_symtable;

/**
 * @brief List of three address code instructions
 */
extern TACList *tac_list;

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

/**
 * @brief Destructor of global entities
 */
void global_free();
#endif // __GLOBAL_H__
