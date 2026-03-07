#ifndef SIMPLESHELL
#define SIMPLESHELL

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEPARATORS " \t\n"
#define BUFFER_SIZE 1024
#define MAX_ARGS 64

// TODO: Create descriptions for each function
FILE *openFile(char **);
char *allocateString(int);
void setShell(char *);
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
