/*
 * scanner.c
 * Ondřej Míchal <xmicha80>
 * 03/10/2020
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "scanner-private.h"

/**
 * @brief scanner_init initializes an instance of scanner
 *
 * @param f a file the scanner will operate on
 *
 * @return an instance of scanner (or NULL on error)
 */
scanner_t *scanner_new(FILE *f) {
  scanner_t *s = malloc(sizeof(struct scanner));
  if (s == NULL)
    return NULL;

  s->file = f;
  s->position = 0;

  return s;
}

/**
 * @brief scanner_free destroys an instance of scanner
 *
 * @param s an instance of scanner
 */
void scanner_free(scanner_t *s) {
  if (s != NULL)
    free(s);
}

/**
 * @brief scanner_scan reads a source file and returns a single token.
 * 
 * @details scanner_scan only returns a single token for each call, thus it has
 * to be called several times until it reaches EOF or encounters an error.
 * The implementation uses finite state machine.
 * 
 * @param s an instance of scanner
 * @param t a pointer to token
 * @param l char array holding the literal string representation of the token.
 * Should be at least of size BUFSIZ.
 * 
 * @return success or EOF
 *
 * @retval 0 success
 * @retval 1 lexical analysis error
 * @retval EOF end of file
 */
int scanner_scan(scanner_t *s, token_t *t, char *l) {
  int i = 0;
  char literal[BUFSIZ];

  // Get the passed token to its initial state
  t->type = INVALID;
  t->attribute = NULL;

  // Skip comments and whitespace characters
  scanner_skip_whitespace_comments(s);

  // Token State Machine
  // TODO: Actually implement the state machine :)
  do {
    // Get the next character
    s->character = fgetc(s->file);

    if (s->character == EOF)
      break;

    literal[i] = s->character;

    switch (s->state) {
    case INIT:
      switch(s->character) {
      case '+':
      case '-':
      case '/':
      case '*':
      case '<':
      case '>':
      case '=':
      case ':':
      case '&':
      case '|':
      case '_':
      case '0' ... '9':
      case 'a' ... 'z':
        break;
      }
      break;
    }
  } while (!isspace(s->character));

  // The owner of literal should handle its lifetime
  if (i < BUFSIZ)
    strcpy(l, literal);

  if (s->character == EOF)
    return EOF;

  return 0;
}
