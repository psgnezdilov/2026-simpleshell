#ifndef SIMPLESHELL
#define SIMPLESHELL

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEPARATORS " \t\n"
#define BUFFER_SIZE 1024
#define MAX_ARGS 64

extern char **environ;

struct execModifiers {
    // -1 => not to use it, other is the index of the sign inside the argument array
    int inFile;
    int outFile;
    int bgExec;
    int appendFile;
};

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
void forkAndExec(char **);
void checkForModifiers(struct execModifiers *, char **);


#endif

/*
Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.
*/