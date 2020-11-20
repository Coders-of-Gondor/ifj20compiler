/**
 * @file scanner-private.h
 * @author Ondřej Míchal <xmicha80>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Header file for scanner-private.c that defines states of the DFA
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 03/10/2020
 */

#include <stdio.h>
#include "scanner.h"
#include "str.h"

typedef enum scanner_state
{
    INIT, //rename to 'Start'?
    f3,   //enter this state after '<' was read from INIT state
    f5,   //enter this state after '>' was read from INIT state
    f7,   //enter this state after '=' was read from INIT state
    f10,  //enter this state after [_a-zA-Z] was read from INIT state
    f14,  //enter this state after [1-9] was read from INIT state
    f15,  //enter this state after '0' was read from INIT state

    q1,   //enter this state after '!' was read from INIT state
    q2,   //enter this state after ':' was read from INIT state

    q3,   //enter this state after '"' was read from INIT state
            //or [\x20-x5Bx\5D-x\7F] was read from q3 state
            //or 'n', 't', '\' or '"' was read from q4 state
            //or [0-9a-fA-F] was read from q6 state

    q4,   //enter this state after '\' was read from q3 state
    q5,   //enter this state after 'x' was read from q4 state
    q6,   //enter this state after [0-9a-fA-F] was read from q5 state
    q7,   //TODO
    q8,   //TODO
    q9,   //TODO
    f12,  //TODO
    f13,  //TODO
    f16,  //TODO
    f17,  //TODO

    STOP,   //finite state machine is in final step
    LEX_ERROR,
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
        CLEAN, /**< the reader is not in a comment (or may not become) */
        COMMENT_CHANGE, /**< the next character could open/close a comment */
        INLINE_COMMENT, /**< the next character is a part of an inline comment */
        BLOCK_COMMENT, /**< the next character is a part of a block comment */
        BLOCK_COMMENT_LEAVING, /**< the next character may end a block comment */
        BLOCK_EXITED /**< current character closed a block comment */
} comment_state;

void scanner_skip_whitespace_comments(scanner_t *s, bool *eol_encountered, int *line);

int innit_scan(scanner_t *s, token_t *t);
int scan_num_lit(scanner_t *s, token_t *t);
int scan_string_lit(scanner_t *s);

