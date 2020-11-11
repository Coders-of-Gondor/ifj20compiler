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
 * @brief Global definition of the variable keywords_symtable.
 */
symtable_t *keywords_symtable;

int global_init() {
  debug_entry();

  // Inicialize global hashtable (resp. symtable) of keywords
  struct keyword keywords[] = { 
    { "else", ELSE },
    { "float64", FLOAT64_LIT },
    { "for", FOR },
    { "func", FUNC },
    { "if", IF },
    { "int" , INT_LIT },
    { "package", PACKAGE },
    { "return", RETURN },
    { "string", STRING_LIT }
  };
  int num_of_keywords = 4;

  keywords_symtable = symtable_new();
  if (keywords_symtable == NULL)
    return 1;

  for (int i = 0; i < num_of_keywords; i++) {
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
