#ifndef SIMPLESHELL
#define SIMPLESHELL

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#define SEPARATORS " \t\n"
#define BUFFER_SIZE 1024
#define MAX_ARGS 64

/* test */
FILE *openFile(char **);
// test
char *allocateString(int);
void setShell();
void tokenize(char *, char **);
void cwdToPrompt(char *);
bool isInternal(const char *command);
void execInternal(char **args);
void cd(char **);
void dir(char **);
void env();
void echo(char **args);
void help();
void pauseShell();

extern char **environ;

#endif
