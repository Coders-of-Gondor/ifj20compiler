/* error.h
 * Vojtech Fiala <xfiala61>
 * FIT BUT
 * 26/10/2020
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#define SUCCESS 0                   // No errors
#define ERROR_LEXICAL 1             // Lexeme error
#define ERROR_SYNTAX 2              // Error in syntax
#define ERROR_SEM_VAR 3             // Semantic error - undefined variable, attempt to redefine variable...
#define ERROR_SEM_VAR_TYPE 4        // Semantic error - invalid data type of newly defined variable
#define ERROR_SEM_COMPATIBILITY 5   // Semantic error - incompatible data types in arithmetic or string operations, or relational expression
#define ERROR_SEM_PROGRAM 6         // Semantic error - invalid number/types of parameters and/or return values
#define ERROR_SEM 7                 // Other semantic errors
#define ERROR_ZERO 9                // Semantic error - dividing by zero
#define ERROR_INTERNAL 99           // Internal error - memory errors...

#endif  //__ERROR_H__

