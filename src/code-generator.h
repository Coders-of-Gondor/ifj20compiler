/**
 * @file code-generator.h
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

char *conversion(char *str);
void generate();
void generate_head();

void build_in_input(char type);
void build_in_float_to_int();
void build_in_int_to_float();
void build_in_len();
void build_in_substr();
void build_in_ord();
void build_in_chr();
