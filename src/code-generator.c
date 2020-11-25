/**
 * @file scanner.c
 * @author Vojtěch Bůbela <xbubel08>
 * @author Vojtěch Fiala <xfiala61>
 * @brief Code Generator
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 25/11/2020
 */

#include <stdio.h>
#include "code-generator.h"
#include <string.h>

#define SPACE 32
#define HASH 35
#define BACKSLASH 92

char *conversion(char *str) {

    int length = strlen(str);
    char buff[5];
    char *new = malloc(4*length+1);
    strcpy(new, "");

    if (new == NULL) 
        exit(99);

    for (int i = 0; i < length; i++) {
        if (str[i] <= SPACE || str[i] == HASH || str[i] == BACKSLASH) {     // <= Space means that everything below space is to be replaced
            sprintf(buff, "\\%d", str[i]);
            new = strncat(new, buff, 4);
        } else {
            new = strncat(new, &str[i], 1);
        }
    }
    return new;

}

void generate() {
    //deklarace pomocnych promenych

    //promenna ve ktere bude jeden radek 3Ak

    generate_head();

    /* while (there are instructions) {
        
        nacti novy radek 3AK

        zpracuj ho

    }

    */

}

void generate_head() {
    printf(".IFJcode20\n");
    printf("JUMP main");
}

void build_in_input(char type) {        // type is to differentiate between inputi, inputs, inputf
    switch (type) {
        case 's':

            break;

        case 'i':

            break;

        case 'f':

            break;
    }
}
