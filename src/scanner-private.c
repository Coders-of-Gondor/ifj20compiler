/**
 * @file scanner-private.c
 * @author Ondřej Míchal <xmicha80>
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief File with private scanner functions.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 03/10/2020
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "scanner-private.h"
#include "error.h"

/**
 * @brief file_move moves current position in a file by X
 * 
 * @param f a pointer to a file
 * 
 * @param number amount by how much we want to move
 */

/**
 * @brief scanner_skip_whitespace_comments skips all whitespace characters and
 * comments until reaching the next potential token
 *
 * @param s an instance of scanner
 */
int scanner_skip_whitespace_comments(scanner_t *s, bool *eol_encountered, int *line) {
    comment_state state = CLEAN;
    char prev_char;

    do {
        prev_char = s->character;
        s->character = fgetc(s->file);
        s->position++;

        if (state == BLOCK_EXITED)
            state = CLEAN;

        switch (s->character) {
            case EOF:
                if (state == BLOCK_COMMENT)
                    return 1;
                break;
            case '\n':
                *eol_encountered = true;
                (*line)++;

                if (state == INLINE_COMMENT)
                    state = CLEAN;
                break;
            case '/':
                if (state == CLEAN) {
                    state = COMMENT_CHANGE;
                } else if (state == BLOCK_COMMENT_LEAVING) {
                    state = BLOCK_EXITED;
                } else if (state == COMMENT_CHANGE) {
                    state = INLINE_COMMENT;
                }
                break;
            case '*':
                if (state == COMMENT_CHANGE) {
                    state = BLOCK_COMMENT;
                } else if (state == BLOCK_COMMENT) {
                    char lil_peek = fgetc(s->file);
                    ungetc(lil_peek, s->file);
                    if (lil_peek ==  '/') {
                        state = BLOCK_COMMENT_LEAVING;
                    }

                }
                break;
            default:
                if (state == COMMENT_CHANGE) {
                    ungetc(s->character, s->file);
                    s->character = prev_char;
                    state = CLEAN;
                }
                break;
        }
    } while(isspace(s->character) || state != CLEAN);

    ungetc(s->character, s->file);
    s->position--;
    return 0;
}

/**
 * @brief scan_string_lit is a branch of DKA that looks for string tokens
 * 
 * @param s is an instance of scanner
 * @param t is a pointer to token
 * 
 * @return returns success or error
 * 
 * @retval 0 on success
 * @retval 1 on error
 * @retval 2 is sign to stop scanner and return token
 */
int scan_string_lit(scanner_t *s) {
    if (s->character == '"') {
        s->state = STOP;
        return 2;
    } else if ((s->character >= 32 && s->character <= 33) ||
               (s->character >= 35 && s->character <= 91) ||
               (s->character >= 93 && s->character <= 126)) {
                    return 0;
    } else if (s->character == 92) {
        s->state = q4;
        return 0;
    }

    return 1;
}
 
/**
 * @brief scan_num_lit is a branch of DKA that looks for numerical tokens
 * 
 * @param s is an instance of scanner
 * 
 * @return returns success or error
 * 
 * @retval 0 on success
 * @retval 1 on error
 */
int scan_num_lit(scanner_t *s, token_t *t) {
    switch (s->character) {

        case '0' ... '9':
            return 0;
            break;

        case '.':
            t->type = FLOAT64_LIT;
            s->state = q7;
            return 0;
            break;

        case 'e':
            t->type = FLOAT64_LIT;
            s->state = q8;
            return 0;
            break;

        case 'E':
            t->type = FLOAT64_LIT;
            s->state = q8;
            return 0;
            break;

        default:
            break;
    }

  return 1;
}

/**
 * @brief innit_scan determines what happens after reading the first character of a new token
 * 
 * @param s is an instance of scanner
 * @param t is a pointer to token
 * 
 * @return returns success or error
 * 
 * @retval 0 on success
 * @retval 1 on error
 */

int innit_scan(scanner_t *s, token_t *t) {
    char peek;
  
    switch(s->character) {
        case ',':
            t->type = COMMA;
            s->state = STOP;
            break;

        case ';':
            t->type = SEMICOLON;
            s->state = STOP;
            break;

        case '(':
            t->type = LPAREN;
            s->state = STOP;
             break;

        case ')':
            t->type = RPAREN;
            s->state = STOP;
            break;

        case '{':
            t->type = LBRACE;
            s->state = STOP;
            break;

        case '}':
            t->type = RBRACE;
            s->state = STOP;
            break;

        case '+':
            t->type = ADD;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = ADD_ASSIGN;
            } else {
                ungetc(peek, s->file);
            }
            break;

        case '-':
            t->type = SUB;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = SUB_ASSIGN;
            } else {
                ungetc(peek, s->file);
            }
            break;

        case '/':
            t->type = DIV;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = DIV_ASSIGN;
            } else {
                ungetc(peek, s->file);
            }            
            break;

        case '*':
            t->type = MUL;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = MUL_ASSIGN;
            } else {
                ungetc(peek, s->file);
            }            
            break;

        case '<':
            t->type = LSS;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = LEQ;
            } else {
                ungetc(peek, s->file);
            }
            break;

        case '>':
            t->type = GTR;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = GEQ;
            } else {
                ungetc(peek, s->file);
            }
            break;

        case '=':
            t->type = ASSIGN;
            s->state = STOP;

            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = EQL;
            } else {
                ungetc(peek, s->file);
            }
            break;

        case ':':
            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = DEFINE;
                s->state = STOP;
            } else {
                ungetc(peek, s->file);
                s->state = LEX_ERROR;
            }
            break;

        case '!':
            peek = fgetc(s->file);

            if (peek == '=') {
                s->character = peek;
                t->type = NEQ;
                s->state = STOP;
            } else {
                ungetc(peek, s->file);
                s->state = LEX_ERROR;
            }
            break;

        case '_':
        case 'a' ... 'z':
        case 'A' ... 'Z':
            t->type = IDENT;
            s->state = f10;
            break;

        case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9':
            t->type = INT_LIT;
            s->state = f14;
            break;

        case '0':
            t->type = INT_LIT;
            s->state = f15;
            break;

        case '"':
            t->type = STRING_LIT;
            s->state = q3;
            break;

        default:
            s->state = LEX_ERROR;
            break;
        }

    if (s->state == LEX_ERROR)
        return 1;

    return 0;
}

