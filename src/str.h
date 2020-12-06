/**
 * @file str.h
 * @author BUT FIT + Marek Filip <xfilip46>
 * @brief hlavickovy soubor pro praci s nekonecne dlouhymi retezci
 * @details Library included from "jednoduchy_interpret.zip" located at the project's
 * website. We do not own anything from this file.
 */

#ifndef __STR_H__
#define __STR_H__

#include <stdint.h>

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

/**
 * @author Marek Filip <xfilip46>
 * @brief Additional string functions working with char pointer.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 6/12/2020
 */

/**
 * @brief Prepend a string with given string.
 * @post Need to free the allocated space.
 */
char *prepend(char *s, const char* t);

/**
 * @brief Append a string with given string.
 * @post Need to free the allocated space.
 */
char *append(char *s, const char* t);

/**
 * @brief Convert double to string.
 * @post Need to free the allocated space.
 */
char *convert_double_to_string(double val);

/**
 * @brief Convert double to string.
 * @post Need to free the allocated space.
 */
char *convert_double_to_hexa(double val);

/**
 * @brief Convert an integer value to string representation.
 * @post Need to free the allocated space.
 */
char *convert_int_to_string(int64_t val);

#endif
