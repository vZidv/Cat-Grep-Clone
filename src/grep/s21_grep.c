#include "s21_grep.h"

#include <locale.h>

int main(int argc, char *argv[]) {
  int result = 0;
  setlocale(LC_ALL, "");
  GrepFlags *flags = calloc(1, sizeof(GrepFlags));
  // char **files = calloc(argc - 2, sizeof(char *));
  //  char **findWord = calloc(argc - 2, sizeof(char *));
  char **findWord = NULL;
  char **files = NULL;

  int filesCount = 0;
  int findWordCount = 0;

  if (!flags) {
    printf("Error allocate memory!\n");

    FreeMemory(flags, files, findWord, filesCount, findWordCount);

    result = 1;
  } else {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (CheckFlag(flags, argv[i])) {
          printf("s21_grep: unknown option -- %s\n", argv[i]);
          result = 1;
          break;
        }
        if (flags->e) {
          if (FlagSetParametr('e', argv, i, &findWord, &findWordCount)) {
            result = 1;
            break;
          }
          flags->e = false;
        }
        if (flags->f) {
          char **filesFindWord = NULL;
          int filesFindWordCount = 0;
          if (FlagSetParametr('f', argv, i, &filesFindWord,
                              &filesFindWordCount)) {
            for (int j = 0; j < filesFindWordCount; j++) {
              free(filesFindWord[j]);
            }
            free(filesFindWord);
            result = 1;
            break;
          }
          if (ReadFileToStrArray(filesFindWord[0], &findWord, &findWordCount)) {
            for (int j = 0; j < filesFindWordCount; j++) {
              free(filesFindWord[j]);
            }
            free(filesFindWord);
            result = 1;
            break;
          }
          for (int j = 0; j < filesFindWordCount; j++) {
            free(filesFindWord[j]);
            filesFindWord[j] = NULL;
          }
          free(filesFindWord);
          filesFindWord = NULL;

          flags->f = false;
        }
        continue;
      }
    }
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][strlen(argv[i]) - 1] == 'e' ||
            argv[i][strlen(argv[i]) - 1] == 'f') {
          ++i;
        }
        continue;
      }

      if (!findWordCount || i == 1) {
        if (AllocateAndCopy(&findWord, &findWordCount, argv[i])) {
          result = 1;
          break;
        }
      } else {
        if (AllocateAndCopy(&files, &filesCount, argv[i])) {
          result = 1;
          break;
        }
      }
    }
    if (!filesCount || !findWordCount) {
      result = 1;
    }
    if (!result) {
      bool lastcharNewLine = true;
      bool manyfiles = filesCount > 1 ? true : false;
      for (int i = 0; i < filesCount; i++) {
        (PrintFile(findWord, findWordCount, files[i], flags, manyfiles,
                   &lastcharNewLine));
      }
    }

    FreeMemory(flags, files, findWord, filesCount, findWordCount);
  }

  return result;
}
int ReadFileToStrArray(char *filename, char ***array, int *arrayCount) {
  int result = 0;
  if (!IsOpen(filename)) {
    printf("s21_grep: %s: No such file or directory", filename);
    result = 1;
  } else {
    FILE *file = fopen(filename, "r");
    char *buffer = NULL;

    while ((buffer = readLine(file)) != NULL) {
      size_t len = strlen(buffer);
      if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
      }

      if (buffer[0] == '\0') {
        free(buffer);
        continue;
      }

      if (AllocateAndCopy(array, arrayCount, buffer)) {
        result = 1;
        free(buffer);
        break;
      }
      free(buffer);
    }

    fclose(file);
  }
  return result;
}
char *readLine(FILE *file) {
  char *result = NULL;

  int size = 256;
  char *buffer = malloc(size * sizeof(char));
  if (!buffer) {
    printf("Error allocate memory!\n");
  } else {
    int len = 0;
    while (fgets(buffer + len, size - len, file)) {
      len += strlen(buffer + len);
      if (buffer[len - 1] == '\n') {
        buffer[len] = '\0';
        break;
      }

      size *= 2;
      char *newBuffer = realloc(buffer, size);
      if (!newBuffer) {
        printf("Error allocate memory!\n");
        free(buffer);
        result = NULL;
        break;
      }
      buffer = newBuffer;
    }
    result = buffer;
    if (len == 0 && feof(file)) {
      free(buffer);
      result = NULL;
    }
  }
  return result;
}

int AllocateAndCopy(char ***array, int *index, char *source) {
  int result = 0;

  *array = realloc(*array, (*index + 1) * sizeof(char *));

  char *copy = myStrdump(source);
  if (!copy) {
    printf("Error duplicating string!\n");
    result = 1;
  } else {
    (*array)[*index] = copy;
    (*index)++;
  }
  return result;
}

char *myStrdump(char *sourse) {
  char *result = NULL;
  if (sourse == NULL)
    result = NULL;
  else {
    int len = strlen(sourse);

    char *str = (char *)malloc(len + 1);
    if (str == NULL) {
      printf("Error allocate memory!\n");
    } else {
      strcpy(str, sourse);
      result = str;
    }
  }

  return result;
}
void FreeMemory(GrepFlags *flags, char **files, char **findWord, int filesCount,
                int findWordCount) {
  if (flags) {
    free(flags);
    flags = NULL;
  }

  if (files) {
    for (int i = 0; i < filesCount; i++) {
      free(files[i]);
      files[i] = NULL;
    }
    free(files);
    files = NULL;
  }
  if (findWord) {
    for (int i = 0; i < findWordCount; i++) {
      free(findWord[i]);
      findWord[i] = NULL;
    }
    free(findWord);
    findWord = NULL;
  }
}

int FlagSetParametr(char flagChar, char *argv[], int index,
                    char ***flagParametr, int *flagParametrsCount) {
  int result = 0;
  char *flagPos = strchr(argv[index], flagChar);

  if (flagPos && *(flagPos + 1) != '\0') {
    char *str = malloc(strlen(flagPos + 1) + 1);
    if (str) {
      strcpy(str, flagPos + 1);
      if (AllocateAndCopy(flagParametr, flagParametrsCount, str)) {
        free(str);
        result = 1;
      }
      free(str);
    } else {
      printf("Error allocate memory!\n");
      result = 1;
    }
  } else {
    if (argv[index + 1] != NULL) {
      if (AllocateAndCopy(flagParametr, flagParametrsCount, argv[index + 1]))
        result = 1;
    }
  }
  return result;
}

int CheckFlag(GrepFlags *flags, char *flag) {
  int errorFlag = 0;
  for (size_t i = 1; i < strlen(flag); i++) {
    switch (flag[i]) {
      case 'e':
        flags->e = true;
        return errorFlag;
        break;
      case 'i':
        flags->i = true;
        break;
      case 'v':
        flags->v = true;
        break;
      case 'c':
        flags->c = true;
        break;
      case 'l':
        flags->l = true;
        break;
      case 'n':
        flags->n = true;
        break;
      case 'h':
        flags->h = true;
        break;
      case 's':
        flags->s = true;
        break;
      case 'f':
        flags->f = true;
        return errorFlag;
        break;
      case 'o':
        flags->o = true;
        break;
      default:
        errorFlag = 1;
        break;
    }
  }

  return errorFlag;
}
int PrintFile(char **findWord, int findWordCount, char *filename,
              GrepFlags *flags, bool manyFiles, bool *lastcharNewLine) {
  int result = 0;
  if (!IsOpen(filename)) {
    if (!flags->s)
      printf("s21_grep: %s: No such file or directory\n", filename);
    result = 1;
  } else {
    FILE *file = fopen(filename, "r");

    int lineCount = 0;
    int lineNumber = 0;
    char *buffer = NULL;

    regex_t regex[findWordCount];
    int reti;

    reti = (RegexCompToArray(regex, findWord, findWordCount, flags));
    if (reti) {
      fclose(file);
      result = 1;
    } else {
      while ((buffer = readLine(file)) != NULL) {
        lineNumber++;
        bool matchFound = false;

        bool isEmptyLine = (buffer[0] == '\n' || buffer[0] == '\0');
        bool printEmptyLine = false;
        for (int j = 0; j < findWordCount; j++) {
          if (isEmptyLine && strcmp(findWord[j], "^$") == 0) {
            matchFound = true;
            printEmptyLine = true;
            break;
          }
          reti = regexec(&regex[j], buffer, 0, NULL, 0);
          if (reti == 0) {
            matchFound = true;
            break;
          }
        }

        if ((matchFound && !flags->v) || (!matchFound && flags->v)) {
          if (isEmptyLine && !flags->v && !printEmptyLine) {
            free(buffer);
            continue;
          }
          if (flags->l) {
            printf("%s\n", filename);
            free(buffer);
            break;
          } else {
            if (flags->c) {
              lineCount++;
              free(buffer);
              continue;
            } else if (flags->o) {
              HandleFountWord(buffer, flags, regex, findWordCount, filename,
                              manyFiles, lineNumber, lastcharNewLine);
            } else {
              if (manyFiles && !flags->h) {
                printf("%s:", filename);
              }
              if (flags->n) {
                printf("%d:", lineNumber);
              }
              printf("%s", buffer);
              if (buffer[strlen(buffer) - 1] != '\n' &&
                  buffer[strlen(buffer) - 1] != '\0')
                printf("\n");
              *lastcharNewLine = (buffer[strlen(buffer) - 1] == '\n');
            }
          }
        }

        free(buffer);
      }

      if (flags->c && !flags->l) {
        if (manyFiles && !flags->h) {
          printf("%s:", filename);
        }
        printf("%d\n", lineCount);
      }

      for (int j = 0; j < findWordCount; j++) {
        regfree(&regex[j]);
      }

      fclose(file);
    }
  }

  return result;
}

int RegexCompToArray(regex_t *regex, char **source, int arrayCount,
                     GrepFlags *flags) {
  int result = 0;
  for (int j = 0; j < arrayCount; j++) {
    result = flags->i ? regcomp(&regex[j], source[j], REG_ICASE | REG_EXTENDED)
                      : regcomp(&regex[j], source[j], REG_EXTENDED);

    if (result) break;
  }
  return result;
}

void HandleFountWord(char *buffer, GrepFlags *flags, regex_t regex[],
                     int findWordCount, char *filename, bool manyFiles,
                     int lineNumber, bool *lastcharNewLine) {
  int occurrentCount = 0;
  int reti;
  regmatch_t pmatch[1];

  while (*buffer != '\0') {
    bool isWordFound = false;

    for (int i = 0; i < findWordCount; i++) {
      reti = regexec(&regex[i], buffer, 1, pmatch, 0);
      if (reti == 0) {
        isWordFound = true;
        occurrentCount++;
        break;
      } else if (reti != REG_NOMATCH) {
        char msgbuf[100];
        regerror(reti, &regex[i], msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return;
      }
    }

    if (isWordFound) {
      if (manyFiles && !flags->h) {
        printf("%s:", filename);
      }

      if (flags->n) {
        printf("%d:", lineNumber);
      }

      printf("%.*s\n", (int)(pmatch[0].rm_eo - pmatch[0].rm_so),
             buffer + pmatch[0].rm_so);
      buffer += pmatch[0].rm_eo;
      *lastcharNewLine = true;
    } else {
      buffer++;
    }
  }
}

int IsRegex(char *findWod) {
  int result = 0;
  char *specialChars = ".*+?|^$[]{}()";

  for (char *p = findWod; *p != '\0'; p++) {
    if (strchr(specialChars, *p) != NULL) {
      result = 1;
      break;
    }
  }

  return result;
}

int IsOpen(char *filename) {
  int result = 1;
  struct stat fileStat;
  memset(&fileStat, 0, sizeof(fileStat));

  if (stat(filename, &fileStat) != 0 || S_ISDIR(fileStat.st_mode)) {
    result = 0;
  }

  FILE *file = fopen(filename, "r");
  if (!file) {
    result = 0;
  } else {
    fclose(file);
  }

  return result;
}

char *mystrcasestr(char *str, char *subStr) {
  char *result = NULL;

  int count = strlen(subStr);
  for (int i = 0; str[i] != '\0'; i++) {
    int j = 0;
    while (toLower(str[i + j]) == toLower(subStr[j]) && subStr[j] != '\0') j++;

    if (j == count) {
      result = &str[i];
      break;
    }
  }
  return result;
}

char toLower(char c) {
  if (c >= 'A' && c <= 'Z') c += ('a' - 'A');

  return c;
}
