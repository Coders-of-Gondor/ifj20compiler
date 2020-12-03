/**
 * @file three-address-code.h 
 * @author Vojtěch Bůbela <xbubel08>
 * @author Ondřej Míchal <xmicha80>
 * @author Marek Filip <xfilip46>
 * @brief Header file for three adress code structure
 * @details Implementace překladače imperativního jazyka IFJ20.
 * This file was inspired by c201.h file from first homework from IAL
 * @date 10/11/2020
 */

#ifndef __THREE_ADDRESS_CODE_H__
#define __THREE_ADDRESS_CODE_H__

#include "str.h"
#include "token.h"

typedef char* item;

typedef enum operator_type {
    // Operations
    OP_INVALID,
    OP_ADD, // for integers and flaots
    OP_ADD_STRING, // fot concatenation of strings
    OP_SUB, // -
    OP_MUL, // *
    OP_DIV, // /
    OP_IDIV, // / for integers
    OP_ADD_ASSIGN, // +=
    OP_SUB_ASSIGN, // -=
    OP_MUL_ASSIGN, // *=
    OP_DIV_ASSIGN, // /=
    OP_IDIV_ASSIGN, // /= for integers
    OP_DEFINE, // :=
    OP_AND, // &&
    OP_OR,  // ||
    OP_EQL, // ==
    OP_NEQ, // !=
    OP_LSS, // <
    OP_LEQ, // <=
    OP_GTR, // >
    OP_GEQ, // >=
    OP_UNARY_ADD, // x = y
    OP_UNARY_SUB_FLOAT, // x = float@0 - y
    OP_UNARY_SUB_INT, // x = int@0 - y

    // Instructions
    OP_CALL, // jump to label
    OP_CREATE_FRAME, // notion that function will be called
    OP_LABEL,  // label
    OP_RETURN, // notion that declaration of function ends
    OP_MOVE, // move arg2 to arg1
    OP_INC_SCOPE, // increase scope
    OP_DEC_SCOPE, // decrease scope
    OP_LABEL_FUNC, // after label comes declaration of function
    OP_JUMP,    // jump
    OP_JUMPIFEQ,  // jump if ()
    OP_JUMPIFNEQ, // jump if !()
    OP_PRINT // print argument on screen
} operator_type;

typedef struct TAC {
    operator_type op; // type of operation
    item arg1;  // first argument of operation
    item arg2;  // second argument of operation
    item result; // result of operation
    struct TAC *next_row; //pointer to the next row of 3AC
} *TACptr;

//structure to manage linked list TAC
typedef struct TACList{
    TACptr first;   // pointer to the first row of 3AC
    TACptr act; // pointer to the active row of 3AC - the one that is being read
} TACList;

/**
 * @brief initalize linked list for three adress code
 * @param L pointer to list of three adress code rows
 */
TACList *TAC_new();

/**
 * @brief deletes linked list
 * @param L pointer to list of three adress code rows
 */
void TAC_free(TACList *L);

/**
 * @brief insert one row or three adress code
 * @param L pointer to list of three adress code rows
 * @param op_type type of operator from enum specified in global.h
 * @param arg1 first argument
 * @param arg2 second argument
 * @param result what the arguments + operation results to
 * @return returns false if memory allocation fails
 */
bool TAC_insert(TACList *L, operator_type op_type, item arg1, item arg2, item result);

/**
 * @brief creates new row of three adress code and switches activity to it
 * @param L pointer to list of three adress code rows
 * @return returns false if memory allocation fails
 */
bool TAC_create_row(TACList *L);

/**
 * @brief Get the value which will be passed to the function call.
 */
item TAC_create_function_argument(int arg_number);

/**
 * @brief Get the value which will be returned.
 */
item TAC_create_return_value(int ret_number);

/**
 * @brief Get the 3ac version of float64.
 */
item TAC_get_float(double val);

/**
 * @brief Get the 3ac version of int.
 */
item TAC_get_int(int64_t val);

/**
 * @brief Get the 3ac version of string.
 */
item TAC_get_string(string val);

/**
 * @brief Get the 3ac version of identifier.
 */
item TAC_get_identifier(char *val);

/**
 * @brief Convert token's term to appropriate TAC item.
 * @return NULL if invalid token, otherwise TAC item.
 */
item TAC_convert_term(token_t token);

#endif
