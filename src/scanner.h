/**
 * @file scanner.h
 * @author Ondřej Míchal <xmicha80>
 * @author Vojtěch Bůbela <xbubel08>
 * @date 03/10/2020
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include <stdbool.h>
#include "token.h"

struct scanner;
typedef struct scanner scanner_t;

scanner_t *scanner_new(FILE *f);
void scanner_free(scanner_t *s);

int scanner_scan(scanner_t *s, token_t *t, bool *eol_encountered, int *line);

#endif // __SCANNER_H__
