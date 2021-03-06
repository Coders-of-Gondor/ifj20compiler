/**
 * @file scanner.c
 * @author Ondřej Míchal <xmicha80>
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Scanner DFA
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 08/11/2020
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "token.h"
#include "scanner-private.h"
#include "str.h"
#include "error.h"


bool escaped_char = false;
bool hexa = false;
char hexa_seq[2];

/**
 * @brief scanner_init initializes an instance of scanner
 *
 * @param f a file the scanner will operate on
 *
 * @return an instance of scanner (or NULL on error)
 */
scanner_t *scanner_new(FILE *f) {
    debug_entry();
    scanner_t *s = malloc(sizeof(struct scanner));
    if (s == NULL)
        return NULL;

    s->file = f;
    s->position = 0;
    s->state = INIT;

    return s;
}

/**
 * @brief scanner_free destroys an instance of scanner
 *
 * @param s an instance of scanner
 */
void scanner_free(scanner_t *s) {
    debug_entry();
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
 * @param eol_encountered is a bool that stores information that EOL - '\n'
 * been read
 * @return success or EOF
 *
 * @retval 0 success
 * @retval 1 lexical analysis error
 * @retval 99 internal error, failed to allocate memory
 * @retval EOF end of file
 */
int scanner_scan(scanner_t *s, token_t *t, bool *eol_encountered, int *line) {
    debug_entry();
    string str;
    int scan_result;

    if (s == NULL)
        return ERROR_INTERNAL;

    if (s->file == NULL)
        return ERROR_INTERNAL;

    if (t == NULL)
        return ERROR_INTERNAL;

    // Scan initialization
    s->state = INIT;
    token_init(t);
    if (strInit(&str) == 1)
        return ERROR_INTERNAL;

    // Skip comments and whitespace characters
    scan_result = scanner_skip_whitespace_comments(s, eol_encountered, line);
    if (scan_result) {
        strFree(&str);
        return ERROR_LEXICAL;
    }


    // Token State Machine
    // TODO: Actually implement the state machine :)
    do {
        int return_val;

        // Get the next character
        s->character = fgetc(s->file);

        if (s->character == EOF) {
            s->state = STOP;
            t->type = EOF_T;
        }

        switch (s->state) {
        /*
        * initial scan solves all branches of DFA apart from
        * numerical literal, decimal literal, identifier and
        * string literal
        */

            //-----beggining of initial scan------
            case INIT:
                innit_scan(s, t);
                break;
            //-----end of initial scan-----

            /*
            * this part of code is for numerical literal
            */

            //-----beggining of numerical literal scan------
            case f14:
                if (scan_num_lit(s, t) == 1) {
                    ungetc(s->character, s->file);
                    s->state = STOP;
                }
                break;

            case f15:
                if ((s->character == 'e') || (s->character == 'E')) {
                    t->type = FLOAT64_LIT;
                    s->state = q8;
                } else if (s->character == '.') {   //0.X
                    t->type = FLOAT64_LIT;
                    s->state = q7;
                } else if (s->character >= 48 && s->character <= 57) {
                    t->type = INVALID;
                    s->state = LEX_ERROR;
                } else {
                    ungetc(s->character, s->file);
                    t->type = INT_LIT;
                    s->state = STOP;
                }
                break;
            //-----end of numerical literal scan------

            /*
            * this part of code is for decimal literal
            */

            //-----beggining of decimal literal scan-----
            case q7:
                if (s->character >= 48 && s->character <= 57) {
                    s->state = f16;
                } else {
                    ungetc(s->character, s->file);
                    s->state = LEX_ERROR;
                }
                break;

            case q8:
                if (s->character == '+' || s->character == '-') {
                    s->state = q9;
                } else if (s->character >= 48 && s->character <= 57) {
                    s->state = f17;
                } else {
                    s->state = LEX_ERROR;
                }
                break;

            case q9:
                if (s->character >= 48 && s->character <= 57) {
                    s->state = f17;
                } else {
                    ungetc(s->character, s->file);   // is this necessary?
                    s->state = LEX_ERROR;
                }
                break;

            case f16:
                if ((s->character == 'e') || (s->character == 'E')) {
                    s->state = q8;
                }

                else if (s->character >= 48 && s->character <= 57) {
                    s->state = f16;       // If after X.Y we have another number, keep on looping
                }

                else {                  // Else we found another token, so return to its beginning and stop
                    s->state = STOP;
                    ungetc(s->character, s->file);
                }
                break;

            case f17:
                if (!(s->character >= 48 && s->character <= 57)) {
                    s->state = STOP;
                    ungetc(s->character, s->file);
                }
                break;
            //----end of decimal literal scan-----

            /*
            * this part of code is for string literal
            */

            //-----beggining of string literal scan-----
            case q3:
                return_val = scan_string_lit(s);
                if (return_val == 1) {
                    s->state = LEX_ERROR;
                } else if (return_val == 2) {
                    s->state = STOP;
                    if ((strAddChar(&str, s->character)) == 1) {
                        return ERROR_INTERNAL;
                    }
                }
                break;

            case q4:
                if (s->character == 'x') {
                    s->state = q5;
                } else if (s->character == 'n' ||
                           s->character == 't' ||
                           s->character == 92  ||
                           s->character == '"') {
                    // make test for uvozovka
                    s->state = q3;
                } else {
                    s->state = LEX_ERROR;
                }
                break;

            case q5:
                if (!(s->character >= 48 && s->character <= 57) &&
                    !(s->character >= 65 && s->character <= 70) &&
                    !(s->character >= 97 && s->character <= 102)) {
                        s->state = LEX_ERROR;
                } else {
                    s->state = q6;
                }
                break;

            case q6:
                if (!(s->character >= 48 && s->character <= 57) &&
                    !(s->character >= 65 && s->character <= 70) &&
                    !(s->character >= 97 && s->character <= 102)) {
                        s->state = LEX_ERROR;
                } else {
                    s->state = q3;
                }
                break;
            //----end of string literal scan-----

            /*
            * this part of code is for identifiers
            */

            //-----beggining of identifier scan-----
            case f10:
                if (isspace(s->character)) {
                s->state = STOP;
                ungetc(s->character, s->file);
                break;
               } else if ((s->character != '_') &&
                        !(s->character >= 48 && s->character <= 57) &&
                        !(s->character >= 65 && s->character <= 90) &&
                        !(s->character >= 97 && s->character <= 122)) {
                            s->state = STOP;
                            ungetc(s->character, s->file);
                }
                break;
            //-----end of identifier scan-----

            case STOP:
                break;

            default:
                s->state = LEX_ERROR;
                t->type = INVALID;
                break;
            }

        if (s->state != STOP && s->state != LEX_ERROR) {
            if (s->state == q4 && s->character == 92) {
                // skip the '\'
                escaped_char = true;
                continue;
            } else if (s->state == q5) {
                // skip the hexa 'x'
                hexa = true;
                continue;
            } else if (s->state == q6 && hexa) {
                // add the first hexa
                hexa_seq[0] = s->character;
                continue;
            } else if (s->state == q3 && hexa) {
                // add the second hexa character and set it
                hexa_seq[1] = s->character;
                int sequence = (int) strtol(hexa_seq, NULL, 16);
                s->character = sequence;
            } else if (s->state == q3 && escaped_char) {
                // add the '\'
                if (s->character == 'n') {
                    s->character = 10;
                } else if (s->character == 't') {
                    s->character = 9;
                } else if (s->character == 92) {
                    s->character = 92;
                } else if (s->character == '"') {
                    s->character = '"';
                }
            }

            if ((strAddChar(&str, s->character)) == 1) {
                return ERROR_INTERNAL;
            }

            escaped_char = false;
            hexa = false;
        }
    } while (s->state != STOP && s->state != LEX_ERROR);

    token_set_attribute(t, str);
    strFree(&str);

    if (s->character == EOF)
        return EOF;

    if (s->state == LEX_ERROR)
        return ERROR_LEXICAL;

    return 0;
}
