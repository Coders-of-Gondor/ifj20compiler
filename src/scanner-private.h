/* scanner-private.h
 * Ondřej Míchal <xmicha80>
 * 03/10/2020
 */

#include <stdio.h>
#include "scanner.h"

typedef enum scanner_state
{
  INIT
} scanner_state;

struct scanner
{
  FILE *file; /**< the scanned file*/
  int position; /**< reading position*/

  scanner_state state; /**< scanner's state*/

  char character; /**< currently assessed character*/
};

typedef enum comment_state
{
  CLEAN = 0x1, /**< the reader is not in a comment (or may not become) */
  COMMENT_CHANGE = 0x2, /**< the next character could open/close a comment */
  INLINE_COMMENT = 0x4, /**< the next character is a part of an inline comment */
  BLOCK_COMMENT = 0x8, /**< the next character is a part of a block comment */
} comment_state;

void scanner_skip_whitespace_comments(scanner_t *s);
