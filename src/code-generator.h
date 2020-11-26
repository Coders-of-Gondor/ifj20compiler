/**
 * @file scanner.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

char *conversion(char *str);
void generate();
void generate_head();

void build_in_input(char type, char *var);
void build_in_float_to_int(char *new, char *var, char* value);
void built_in_int_to_float(char *new, char *var, char *value);
void built_in_len(char *new, char *var, char *value);
void built_in_substr(char *var, char *content, char *begin, char *length);
