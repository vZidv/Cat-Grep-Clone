#include "s21_cat.h"

#define SIZE_STRIN_BUFFER 2048

int main(int argc, char *argv[]) {
  bool errorflag = false;

  CatFlags *flags = calloc(1, sizeof(CatFlags));
  char **files = calloc(argc - 1, sizeof(char *));
  if (!flags || !files) {
    printf("Error allocate memory\n");
    errorflag = true;
  } else {
    int filesCount = 0;

    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (CheckFlag(flags, argv[i])) {
          errorflag = true;
          break;
        }
        continue;
      } else {
        files[filesCount] = argv[i];
        filesCount++;
      }
    }
    for (int i = 0; i < filesCount; i++) {
      PrintFile(flags, files[i]);
    }
  }

  MemoryFree(flags, files);
  return errorflag;
}
void MemoryFree(CatFlags *flags, char **files) {
  free(flags);
  free(files);
}

int CheckFlag(CatFlags *flags, char *flag) {
  bool errorflag = false;

  if (flag[1] == '-') {
    return GnuFlags(flags, flag);
  }
  for (size_t i = 1; i < strlen(flag); i++) {
    switch (flag[i]) {
      case 'b':
        flags->b = true;
        break;
      case 'e':
        flags->e = true;
        flags->v = true;
        break;
      case 'E':
        flags->e = true;
        break;
      case 'n':
        flags->n = true;
        break;
      case 'v':
        flags->v = true;
        break;
      case 's':
        flags->s = true;
        break;
      case 't':
        flags->t = true;
        flags->v = true;
        break;
      case 'T':
        flags->t = true;
        break;
      default:
        printf("s21_cat: unknown option -- %s\n", flag);
        errorflag = true;
        break;
    }
  }

  return errorflag;
}
int GnuFlags(CatFlags *flags, char *flag) {
  bool errorFlag = false;

  if (!(strcmp(flag, "--number"))) {
    flags->n = true;
  } else if (!(strcmp(flag, "--number-nonblank"))) {
    flags->b = true;
  } else if (!(strcmp(flag, "--squeeze-blank"))) {
    flags->s = true;
  } else {
    printf("Error: Unknown flag '%s'\n", flag);
    errorFlag = true;
  }

  return errorFlag;
}

int PrintFile(CatFlags *flags, char *filename) {
  bool errorflag = false;

  if (IsOpen(filename)) {
    printf("s21_cat: %s: No such file or directory", filename);
    errorflag = true;
  } else {
    FILE *file = fopen(filename, "r");

    char *buffer = malloc(SIZE_STRIN_BUFFER * sizeof(char));
    char *staticBuffer = malloc(SIZE_STRIN_BUFFER * sizeof(char));

    if (buffer == NULL || staticBuffer == NULL) {
      printf("Error allocate memory\n");
      fclose(file);
      errorflag = true;
    } else {
      static int lineNumber = 1;
      bool lastLineEmpty = false;
      static bool firestLineOfNextFile = true;

      size_t countChar = 0;
      while (MyFgets(buffer, SIZE_STRIN_BUFFER, file, &countChar)) {
        strcpy(staticBuffer, buffer);

        bool lineEmpty = (buffer[0] == '\n');
        if (flags->s && lineEmpty && lastLineEmpty) {
          lastLineEmpty = true;
          countChar = 0;
          continue;
        }

        if (flags->v || flags->t) {
          NonPrintCharVis(buffer, &countChar, flags);
        }

        if (flags->n && !flags->b) {
          if (firestLineOfNextFile) {
            printf("%6d\t", lineNumber++);
            firestLineOfNextFile = false;
          } else {
            printf("%6d\t", lineNumber++);
          }
        }
        if (flags->b && !lineEmpty) {
          if (firestLineOfNextFile) {
            printf("%6d\t", lineNumber++);
            firestLineOfNextFile = false;
          } else {
            printf("%6d\t", lineNumber++);
          }
        }

        output(buffer, &countChar, flags);

        lastLineEmpty = lineEmpty;
        countChar = 0;
        // free(buffer);
      }
      firestLineOfNextFile = true;
      free(staticBuffer);
      free(buffer);
      fclose(file);
    }
  }
  return errorflag;
}

void output(char *buffer, size_t *count, CatFlags *flags) {
  size_t i = 0;
  while (i < *count) {
    if (buffer[i] == '\n' && flags->e) putchar('$');
    putchar(buffer[i++]);
  }
}
int MyFgets(char *buffer, int size, FILE *file, size_t *countChar) {
  int i = 0;
  char ch = 0;
  char flag = 1;

  while (flag && (ch = fgetc(file)) != EOF && i < size - 1) {
    buffer[i++] = ch;
    (*countChar)++;
    if (ch == '\n') flag = 0;
  }

  buffer[i] = '\0';
  return (i > 0 || ch == '\n');
}
int IsOpen(char *filename) {
  bool result = false;
  FILE *file = fopen(filename, "r");
  if (file) {
    result = true;
    fclose(file);
  }

  return !result;
}
int RemoveTab(char *buffer) {
  bool errorflag = false;
  int length = strlen(buffer);
  char *result = malloc((length * 2 + 1) * sizeof(char));

  if (!result) {
    printf("Error allocating memory!\n");
    errorflag = true;
  } else {
    int j = 0;
    for (int i = 0; i < length; i++) {
      if (buffer[i] == '\t') {
        result[j++] = '^';
        result[j++] = 'I';
      } else
        result[j++] = buffer[i];
    }
    result[j] = '\0';
    strcpy(buffer, result);
    free(result);
  }

  return errorflag;
}
int NonPrintCharVis(char *buffer, size_t *count, CatFlags *flags) {
  bool errorflag = false;

  size_t i = 0, j = 0, countChanges = 0;
  char *result = malloc(SIZE_STRIN_BUFFER * sizeof(char));
  if (!result) {
    printf("Error allocating memory!\n");
    errorflag = true;
  } else {
    while (i < *count) {
      unsigned char ch = (unsigned char)buffer[i];
      if (ch == 9 && flags->t) {
        result[j++] = '^';
        result[j++] = 'I';
        countChanges += 1;
      } else if (flags->v) {
        if (ch == 0) {
          result[j++] = '^';
          result[j++] = '@';
          countChanges += 1;
        } else if ((ch > 0 && ch <= 8) || (ch >= 11 && ch <= 31)) {
          result[j++] = '^';
          result[j++] = ch + 64;
          countChanges += 1;
        } else if (ch == 127) {
          result[j++] = '^';
          result[j++] = '?';
          countChanges += 1;
        } else if (ch >= 128 && ch <= 159) {
          result[j++] = 'M';
          result[j++] = '-';
          result[j++] = '^';
          result[j++] = ch - 64;
          countChanges += 3;
        } else if (ch >= 160) {
          result[j++] = 'M';
          result[j++] = '-';
          result[j++] = ch - 128;
          countChanges += 2;
        } else {
          result[j++] = ch;
        }
      } else {
        result[j++] = ch;
      }
      i++;
    }
    result[j] = '\0';
    (*count) += countChanges;
    memcpy(buffer, result, *count);
    free(result);
  }

  return errorflag;
}