/* global.c
 * Ondřej Míchal <xmicha80>
 * FIT BUT
 * 11/11/2020
 */

#include "debug.h"
#include "global.h"
#include "symtable.h"
#include "token.h"

/**
 * @brief Definitions of the global variables of global.h
 */
symtable_t *keywords_symtable;
token_t lookahead;
scanner_t *scanner;
bool eol_encountered;

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
    symtable_iterator_t it = symtable_lookup_add(keywords_symtable, keyword.lit);
    if (!symtable_iterator_valid(it))
      return 1;

    // Create a new token for a keyword and add it to the key in symtable
    token_t t;
    t.type = keyword.type;
    t.attribute.sym_key = keyword.lit;
    symtable_iterator_set_value(it, t);
  }

  return 0;
}
