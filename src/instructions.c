/**
 * @file scanner.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Scanner DFA
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 24/11/2020
 */

void print_MOVE(char *var, char *sym, ) {
    printf("MOVE %s@%s %s@%s\n")
}

void print_ADD(char *var, char *sym1, char *sym2) {
    printf("ADD %s %s %s\n", var, sym1, sym2);
}

void print_SUB(char *var, char *sym1, char *sym2) {
    printf("SUB %s %s %s\n", var, sym1, sym2);
}

void print_MUL(char *var, char *sym1, char *sym2) {
    printf("MUL %s %s %s\n", var, sym1, sym2);
}

void print_DIV(char *var, char *sym1, char *sym2) {
    printf("DIV %s %s %s\n", var, sym1, sym2);
}

void print_IDIV(char *var, char *sym1, char *sym2) {
    printf("SUB %s %s %s\n", var, sym1, sym2);
}
