/**
 * @file stack.h
 * @author Marek Filip <xfilip46>
 * @brief General stack header file, make use of macros for the generalization.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * Details of how all the magic works is described below.
 * @date 26/11/2020
 */

// Header guards are dependent on the given TYPE.
#ifdef TYPEDEF
TYPEDEF;
#endif

/********* TYPICAL USAGE *********
#define TYPE <YOURTYPE>
#include "stack.h"
#undef TYPE

...

stack_YOURTYPE_t stack = stack_YOURTYPE_init();
YOURTYPE value = 5;

stack_YOURTYPE_push(stack, value);
stack_YOURTYPE_peek(stack);
YOURTYPE value2 = stack_YOURTYPE_pop(stack, value);

stack_YOURTYPE_free(stack);
// You need to extern the function definitions.
****** END OF TYPICAL USAGE ******/

/* ALTERNATIVE USAGE
 * Create stack_<YOURTYPE>.c source files with extern func definitions
 * and relevant stack_<YOURTYPE>.h with the defined TYPE.
 */

/** START of the STACK OVERFLOW content **/
/**
 * Macro magic to make stack structure take any data type.
 * This content was taken from STACK OVERFLOW network.
 * Followed Stack Overflow guidelines:
 *  - https://stackoverflow.blog/2009/06/25/attribution-required/
 * Original question:
 *  - https://stackoverflow.com/questions/26699505
 * Author of the question:
 *  - SouvikMaji
 *  - https://stackoverflow.com/users/3198508/souvikmaji
 * Author of the relevant answer:
 *  - 5gon12eder
 *  - https://stackoverflow.com/users/1392132/5gon12eder
 */

// Do not include the header guards if you want to use multiple types of stack in one file.
// All of the functions need to be declared as inline, because of the type magic.

// Create the struct name from given parameters e.g. stack_int_t
#define CONCAT_STRUCT_R(A, B, C, D) A ## B ## C ## D
#define CONCAT_STRUCT(A, T) CONCAT_STRUCT_R(A, _, T, _t)

// Create the function name from given parameters e.g. stack_int_init
#define CONCAT_FUNC_R(A, B, C, D, E) A ## B ## C ## D ## E
#define CONCAT_FUNC(A, T, F) CONCAT_FUNC_R(A, _, T, _, F)

typedef struct CONCAT_STRUCT(stack, TYPE)
{
    int size;    /**< Size of the stack */
    int top;     /**< Top of the stack */
    TYPE *array;  /**< Array representaiton of the stack */
} CONCAT_STRUCT(stack, TYPE);

/** END of the STACK OVERFLOW content **/

#include <stdbool.h>
#include <stdlib.h>

// stack's starting capacity
#define STACK_START_SIZE 10
// function pointer to the compare function for _ispresent()
typedef int (*CONCAT_FUNC(CMP, TYPE, PTR))(TYPE, TYPE);

/**
 * @brief Helper function for reszing the stack when its capacity has been filled.
 * @return True if everything went ok. False if internal error happened.
 */
inline bool CONCAT_FUNC(stack, TYPE, resize)(CONCAT_STRUCT(stack, TYPE) *stack) {
    // multiple the size by two
    stack->size *= 2;
    stack->array = (TYPE *) realloc(stack->array, sizeof(TYPE) * stack->size);
    if (stack->array == NULL) {
        return false;
    }
    return true;
}

/**
 * @brief Helper function to tell if the stack is empty.
 */
inline bool CONCAT_FUNC(stack, TYPE, isempty)(CONCAT_STRUCT(stack, TYPE) *stack) {
    return stack->top == -1;
}

/**
 * @brief Helper function to tell if the stack's capacity is full.  */
inline bool CONCAT_FUNC(stack, TYPE, isfull)(CONCAT_STRUCT(stack, TYPE) *stack) {
    return stack->top == stack->size - 1;
}

/**
 * @brief Initialize the stack.
 * @return Pointer to stack struct. NULL if internal error.
 */
inline CONCAT_STRUCT(stack, TYPE) *CONCAT_FUNC(stack, TYPE, init)() {
    CONCAT_STRUCT(stack, TYPE) *stack =
        (CONCAT_STRUCT(stack, TYPE) *) malloc(sizeof(CONCAT_STRUCT(stack, TYPE)));
    if (stack == NULL) {
        return NULL;
    }

    stack->size = STACK_START_SIZE;
    stack->top = -1;  // there is no top value yet
    stack->array = (TYPE *) malloc(sizeof(TYPE) * stack->size);
    if (stack->array == NULL) {
        return NULL;
    }

    return stack;
}

/**
 * @brief Push a value onto the stack.
 * @return True if everything went okay, False if internal error happened.
 */
inline bool CONCAT_FUNC(stack, TYPE, push)(CONCAT_STRUCT(stack, TYPE) *stack, TYPE val) {
    if (CONCAT_FUNC(stack, TYPE, isfull)(stack)) {
        if (!CONCAT_FUNC(stack, TYPE, resize)(stack)) {
            return false;
        }
    }

    stack->top++;
    stack->array[stack->top] = val;
    return true;
}

/**
 * @brief Read the top value from the stack.
 */
inline TYPE CONCAT_FUNC(stack, TYPE, peek)(CONCAT_STRUCT(stack, TYPE) *stack) {
    return stack->array[stack->top];
}

/**
 * @brief Read a value from the stack and remove it.
 */
inline TYPE CONCAT_FUNC(stack, TYPE, pop)(CONCAT_STRUCT(stack, TYPE) *stack) {
    TYPE popped = stack->array[stack->top];
    stack->top--;
    return popped;
}

/**
 * @brief Reverse the order of values in stack
 */
inline void CONCAT_FUNC(stack, TYPE, reverse)(CONCAT_STRUCT(stack, TYPE) *stack) {
    for (int i = 0; i <= stack->top / 2; i++) {
        TYPE tmp = stack->array[i];
        stack->array[i] = stack->array[stack->top-i];
        stack->array[stack->top] = tmp;
    }
}

/**
 * @brief Find out if a value is already in the stack.
 * @pre Needs a comparison function as its parameter.
 *      Comparsion function returns 0 if the comparsion was succesful.
 * @return true if value found, false if not.
 */
inline bool CONCAT_FUNC(stack, TYPE, ispresent)(CONCAT_STRUCT(stack, TYPE) *stack,
                        TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR) compare_func) {
    int top = stack->top;

    while (top >= 0) {
        if (compare_func(stack->array[top], value_to_find) == 0) {
            // value found
            return true;
        } else {
            top--;
        }
    }

    // no value found
    return false;
}

/**
 * @brief Tell how many times is the value in the stack.
 * @pre Needs a comparison function as its parameter.
 *      Comparsion function returns 0 if the comparsion was succesful.
 * @return Return the number of occurences.
 */
inline int CONCAT_FUNC(stack, TYPE, howmany)(CONCAT_STRUCT(stack, TYPE) *stack,
                        TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR) compare_func) {
    int top = stack->top;
    int occurences = 0;

    while (top >= 0) {
        if (compare_func(stack->array[top], value_to_find) == 0) {
            // value found
            occurences++;
        }
        top--;
    }

    return occurences;
}

/**
 * @brief Get the value located at given index.
 * @return Return the value located at the index.
 */
inline TYPE CONCAT_FUNC(stack, TYPE, at)(CONCAT_STRUCT(stack, TYPE) *stack, int index) {
    return stack->array[index];
}

/**
 * @brief Get the index of the first value encountered.
 * @pre Needs a comparison function as its parameter.
 *      Comparsion function returns 0 if the comparsion was succesful.
 * @return index if found, -1 if value not found.
 */
inline int CONCAT_FUNC(stack, TYPE, find)(CONCAT_STRUCT(stack, TYPE) *stack,
        TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR) compare_func) {
    int top = stack->top;

    while (top >= 0) {
        if (compare_func(stack->array[top], value_to_find) == 0) {
            // token found
            return top;
        } else {
            top--;
        }
    }

    // searched value not found
    return -1;  // token_type starts at 0, so this is safe
}

/**
 * @brief Destroy the stack struct and deallocate the memory.
 * @details Set stack to null.
 */
inline void CONCAT_FUNC(stack, TYPE, free)(CONCAT_STRUCT(stack, TYPE) *stack) {
    free(stack->array);
    free(stack);
}
