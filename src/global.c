/**
 * @file global.c
 * @author Ondřej Míchal <xmicha80>
 * @brief Global variable declarations
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 11/11/2020
 */

#include "debug.h"
#include "global.h"
#include "symtable.h"
#include "three-address-code.h"
#include "token.h"

/**
 * @brief Definitions of the global variables of global.h
 */
symtable_t *keywords_symtable;

TACList *tac_list;

int global_init() {
    debug_entry();

    // Inicialize global hashtable (resp. symtable) of keywords
    struct keyword keywords[] = { 
        { "else", ELSE },
        { "float64", FLOAT64 },
        { "for", FOR },
        { "func", FUNC },
        { "if", IF },
        { "int" , INT },
        { "package", PACKAGE },
        { "return", RETURN },
        { "string", STRING }
    };
    const int NUM_OF_KEYWORDS = sizeof(keywords) / sizeof(struct keyword);

    keywords_symtable = symtable_new();
    if (keywords_symtable == NULL)
        return 1;

    for (int i = 0; i < NUM_OF_KEYWORDS; i++) {
        struct keyword keyword = keywords[i];

        // Add keyword to symtable (only key)
        symtable_symbol_t *symbol = symtable_add_symbol(keywords_symtable, keyword.lit);
        if (symbol == NULL)
            return 1;

        // Create a new token for a keyword and add it to the key in symtable
        token_t t;
        t.type = keyword.type;
        t.attribute.sym_key = keyword.lit;

        symbol->type = IDENT;
        symbol->token = t;
    }

    // Initialize list of three address code instructions
    tac_list = TAC_new();
    if (tac_list == NULL)
        return 1;

    if (!TAC_create_row(tac_list))
        return 1;

    return 0;
}

void global_free() {
    debug_entry();

    symtable_free(keywords_symtable);
    TAC_free(tac_list);
}
