/** 
 * @file error.h
 * @author Vojtěch Fiala <xfiala61>
 * @author Marek Filip <xfilip46>
 * @brief Error codes and function declarations
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#include "token.h"

#define SUCCESS 0                   /**< No errors */
#define ERROR_LEXICAL 1             /**< Lexeme error */
#define ERROR_SYNTAX 2              /**< Error in syntax */
#define ERROR_SEM_VAR 3             /**< Semantic error - undefined variable, attempt to redefine variable... */
#define ERROR_SEM_VAR_TYPE 4        /**< Semantic error - invalid data type of newly defined variable */
#define ERROR_SEM_COMPATIBILITY 5   /**< Semantic error - incompatible data types in arithmetic or string operations, or relational expression */
#define ERROR_SEM_PROGRAM 6         /**< Semantic error - invalid number/types of parameters and/or return values */
#define ERROR_SEM 7                 /**< Other semantic errors */
#define ERROR_ZERO 9                /**< Semantic error - dividing by zero */
#define ERROR_INTERNAL 99           /**< Internal error - memory errors... */

/**
 * @brief Function for exiting the program.
 * @pre Make sure the deallocation of resources is sufficient.
 *
 */
void error_exit(unsigned error_code, const char* fmt, ...);

/**
 * @brief Function to call when a syntax error is encountered.
 */
void throw_syntax_error(token_type type, int line);

/**
 * @brief Function to call when a lexical error is encountered.
 */
void throw_lex_error(int line);

/**
 * @brief Function to call when an internal error is encountered.
 */
void throw_internal_error();

/**
 * @brief Function to call when we succesfully compiled the program.
 */
void success_exit();


#endif  //__ERROR_H__

