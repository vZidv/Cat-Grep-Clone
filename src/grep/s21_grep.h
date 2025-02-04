#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef enum { false, true } bool;
typedef struct {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
} GrepFlags;

void HandleFountWord(char *buffer, GrepFlags *flags, regex_t regex[],
                     int findWordCount, char *filename, bool manyFiles,
                     int lineNumber, bool *lastcharNewLine);
int FlagSetParametr(char flagChar, char *argv[], int index,
                    char ***flagParametr, int *flagParametrsCount);
int PrintFile(char **findWord, int findWordCount, char *filename,
              GrepFlags *flags, bool manyFiles, bool *lastcharNewLine);
void FreeMemory(GrepFlags *flags, char **files, char **findWord, int filesCount,
                int findWordCount);
int ReadFileToStrArray(char *filename, char ***array, int *arrayCount);
int AllocateAndCopy(char ***array, int *index, char *source);
int CheckFlag(GrepFlags *flags, char *flag);
char *mystrcasestr(char *str, char *subStr);
int allFlagsZero(GrepFlags *flags);
char *myStrdump(char *sourse);
char *readLine(FILE *file);
int IsOpen(char *file);
char toLower(char c);
int RegexCompToArray(regex_t *regex, char **source, int arrayCount,
                     GrepFlags *flags);

void SimplLinePrint(char *buffer, char *found, char **findWord, int j,
                    GrepFlags *flags);

int IsRegex(char *findWod);

#endif