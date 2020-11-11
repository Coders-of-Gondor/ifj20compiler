/**
 * Library included from "jednoduchy_interpret.zip" located at the project's
 * website. We do not own anything from this file.
 */

//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

#ifndef __STR_H__
#define __STR_H__

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

#endif
