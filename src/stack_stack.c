/**
 * @file stack_stack.c
 * @author Marek Filip <xfilip46>
 * @brief stack stack source file.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * Generated by generate_stack_type.bash (Author: Marek Filip <xfilip46>)
 * @date 27/11/2020
 */

#include "stack_stack.h"

// due to how C includes work we can safely include extern definitions
// from a header file
#include "stack_extern_template.h"
#undef TYPE  // undef the TYPE used in stack_stack.h
