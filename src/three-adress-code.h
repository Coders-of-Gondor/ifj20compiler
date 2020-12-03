/**
 * @file three-adress-code.h 
 * @author Vojtěch Bůbela <xbubel08>
 * @brief Header file for three adress code structure
 * @details Implementace překladače imperativního jazyka IFJ20.
 * Tento soubor se inspiruje souborem c201.h ze zadaní prvního
 * domácího úkolu předmetu IAL
 * @date 10/11/2020
 */

#ifndef __THREE_ADRESS_CODE_H__
#define __THREE_ADRESS_CODE_H__

typedef char* item;

//types of opearations passed by three adress code to code generator
typedef enum operator_type {
    OP_INVALID, //invalid operator

    //operations
    OP_ADD, // +
    OP_SUB, // -
    OP_MUL, // *
    OP_DIV, // /
    OP_ADD_ASSIGN, // +=
    OP_SUB_ASSIGN, // -=
    OP_MUL_ASSIGN, // *=
    OP_DIV_ASSIGN, // /=
    OP_DEFINE, // :=
    OP_AND, // &&
    OP_OR,  // ||
    OP_EQL, // ==
    OP_NEQ, // !=
    OP_LSS, // <
    OP_LEQ, // <=
    OP_GTR, // >
    OP_GEQ, // >=

    //instructions
    OP_CALL, // jump to label
    OP_CREATE_FRAME, //notion that function will be called
    OP_LABEL, //after label comes declaration of function
    OP_RETURN, //notion that declaration of function ends
    OP_MOVE, //move arg2 to arg1
    OP_INC_SCOPE, //increase scope
    OP_DEC_SCOPE //decrease scope
} operator_type;

//one row of three adress code in quadruples form (from dragon book)
//contains operator, 2 argumetns, result and pointer to the nxt row
//because its linked list
typedef struct TAC {
    operator_type op;
    item arg1;
    item arg2;
    item result;
    struct TAC *next_row;
} *TACptr;

//structure to manage linked list TAC
typedef struct TACList{
    TACptr first;
    TACptr act;
} TACList;


/**
 * @brief deletes linked list
 * 
 * @param L pointer to list of three adress code rows
 */
void TAC_delete(TACList *L);

/**
 * @brief insert one row or three adress code
 * 
 * @param L pointer to list of three adress code rows
 * 
 * @param op_type type of operator from enum specified in global.h
 * 
 * @param arg1 first argument
 * 
 * @param arg2 second argument
 * 
 * @param result what the arguments + operation results to
 */
void TAC_insert(TACList *L, operator_type op_type, char *arg1, char *arg2, char *result);

/**
 * @brief creates new row of three adress code and switches activity to it
 * 
 * @param L pointer to list of three adress code rows
 */
void TAC_create_row(TACList *L);

/**
 * @brief initalize linked list for three adress code
 * 
 * @param L pointer to list of three adress code rows
 */
void TAC_init(TACList *L);

#endif