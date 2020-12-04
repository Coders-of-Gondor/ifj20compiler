/**
 * @file stack_template.h
 * @author Marek Filip <xfilip46>
 * @brief Integer stack header file.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 26/11/2020
 */

// @pre Need to have TYPE defined when included.
// header guards included in stack.h, no need for them here

// Useful functions:
extern inline CONCAT_STRUCT(stack, TYPE) *CONCAT_FUNC(stack, TYPE, init)();
extern inline void CONCAT_FUNC(stack, TYPE, free)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline bool CONCAT_FUNC(stack, TYPE, push)(CONCAT_STRUCT(stack, TYPE) *stack, TYPE val);
extern inline TYPE CONCAT_FUNC(stack, TYPE, peek)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline TYPE CONCAT_FUNC(stack, TYPE, pop)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline void CONCAT_FUNC(stack, TYPE, reverse)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline bool CONCAT_FUNC(stack, TYPE, isempty)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline bool CONCAT_FUNC(stack, TYPE, ispresent)(CONCAT_STRUCT(stack, TYPE) *stack, TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR) compare_func);
extern inline int CONCAT_FUNC(stack, TYPE, howmany)(CONCAT_STRUCT(stack, TYPE) *stack, TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR)compare_func);
extern inline TYPE CONCAT_FUNC(stack, TYPE, at)(CONCAT_STRUCT(stack, TYPE) *stack, int index);
extern inline int CONCAT_FUNC(stack, TYPE, find)(CONCAT_STRUCT(stack, TYPE) *stack, TYPE value_to_find, CONCAT_FUNC(CMP, TYPE, PTR) compare_func);
// Internal functions: (do not use!)
extern inline bool CONCAT_FUNC(stack, TYPE, isfull)(CONCAT_STRUCT(stack, TYPE) *stack);
extern inline bool CONCAT_FUNC(stack, TYPE, resize)(CONCAT_STRUCT(stack, TYPE) *stack);
