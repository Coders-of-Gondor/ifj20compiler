/**
 * Library included from "jednoduchy_interpret.zip" located at the project's
 * website. We do not own anything from this file.
 */

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <malloc.h>
#include "str.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
// funkce vytvori novy retezec
{
    if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
        return STR_ERROR;
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = STR_LEN_INC;
    return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
    free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
    s->str[0] = '\0';
    s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
    if (s1->length + 1 >= s1->allocSize)
    {
        // pamet nestaci, je potreba provest realokaci
        if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
            return STR_ERROR;
        s1->allocSize = s1->length + STR_LEN_INC;
    }
    s1->str[s1->length] = c;
    s1->length++;
    s1->str[s1->length] = '\0';
    return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
    int newLength = s2->length;
    if (newLength >= s1->allocSize)
    {
        // pamet nestaci, je potreba provest realokaci
        if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
            return STR_ERROR;
        s1->allocSize = newLength + 1;
    }
    strcpy(s1->str, s2->str);
    s1->length = newLength;
    return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
    return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
    return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
    return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
    return s->length;
}

/**
 * @author Marek Filip <xfilip46>
 * @brief Additional string functions working with char pointer.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 6/12/2020
 */

char *prepend(char *s, const char *t) {
    size_t len_t = strlen(t);
    size_t len_s = strlen(s);
    char *buf = malloc(len_t + len_s + 1);
    if (buf == NULL) {
        return NULL;
    }

    memmove(buf + len_t, s, len_s + 1);
    memcpy(buf, t, len_t);

    return buf;
}

char *append(char *s, const char *t) {
    size_t len_t = strlen(t);
    size_t len_s = strlen(s);
    char *buf = malloc(len_t + len_s + 1);
    if (buf == NULL) {
        return NULL;
    }

    buf[0] = '\0';
    strcat(buf, s);
    strcat(buf, t);

    return buf;
}

char *convert_double_to_string(double val) {
    // Count how many characters would be needed to print the whole number.
    // Not counting the '\0'.
    size_t bufsz = snprintf(NULL, 0, "%g", val);
    char *buf = malloc(bufsz + 1);
    if (buf == NULL) {
        return NULL;
    }

    snprintf(buf, bufsz + 1, "%g", val);

    return buf;
}

char *convert_int_to_string(int64_t val) {
    // Count how many characters would be needed to print the whole number.
    // Not counting the '\0'.
    size_t bufsz = snprintf(NULL, 0, "%ld", val);
    char *buf = malloc(bufsz + 1);
    if (buf == NULL) {
        return NULL;
    }
    snprintf(buf, bufsz + 1, "%ld", val);

    return buf;
}

char *convert_double_to_hexa(double val) {
    // Count how many characters would be needed to print the whole number.
    // Not counting the '\0'.
    size_t bufsz = snprintf(NULL, 0, "%a", val);
    char *buf = malloc(bufsz + 1);
    if (buf == NULL) {
        return NULL;
    }

    snprintf(buf, bufsz + 1, "%a", val);

    return buf;
}
