#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;
typedef struct {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
} CatFlags;

int MyFgets(char *buffer, int size, FILE *file, size_t *countChar);
int NonPrintCharVis(char *buffer, size_t *count, CatFlags *flags);
void output(char *buffer, size_t *count, CatFlags *flags);
void MemoryFree(CatFlags *flags, char **files);
int PrintFile(CatFlags *flags, char *filename);
int CheckFlag(CatFlags *flags, char *flag);
int GnuFlags(CatFlags *flags, char *flag);
int RemoveTab(char *buffer);
int IsOpen(char *filename);

#endif