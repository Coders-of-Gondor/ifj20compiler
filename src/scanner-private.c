/* scanner-private.c
 * Ondřej Míchal <xmicha80>
 * 03/10/2020
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "scanner-private.h"

/**
 * @brief scanner_skip_whitespace_comments skips all whitespace characters and
 * comments until reaching the next potential token
 *
 * @param s an instance of scanner
 */
void scanner_skip_whitespace_comments(scanner_t *s) {
  comment_state state = CLEAN;

  do {
    s->character = getc(s->file);
    s->position++;
    switch (s->character) {
      case '\n':
        if (state & INLINE_COMMENT)
          state = CLEAN;
        break;
      case '/':
        if (state & CLEAN)
          state = COMMENT_CHANGE;
        else if (state & COMMENT_CHANGE && state & BLOCK_COMMENT)
          state = CLEAN;
        else if (state & COMMENT_CHANGE && !(state & BLOCK_COMMENT))
          state = INLINE_COMMENT;
        break;
      case '*':
        if (state & COMMENT_CHANGE)
          state = BLOCK_COMMENT;
        else if (state & BLOCK_COMMENT)
          state = COMMENT_CHANGE | BLOCK_COMMENT;
        break;
      default:
        // FIXME: I think this needs to be more specific so that a needed character is not lost.
        if (state & COMMENT_CHANGE)
          state = CLEAN;
        break;
    }
  } while(isspace(s->character) || !(state & CLEAN));

  fseek(s->file, -1, SEEK_CUR);
  s->position--;
}
