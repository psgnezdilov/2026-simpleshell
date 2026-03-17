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
    char *inFile;
    char *outFile;
    char *appendFile;
    bool bgExec;
};

FILE *openFile(char **);
char *allocateString(int);
void setShell(char *);
void tokenize(char *, char **);
void cwdToPrompt(char *);
bool isInternal(const char *command);
void execInternal(char **args, struct execModifiers);
void cd(char **);
void dir(char **);
void env();
void echo(char **args);
void help(struct execModifiers);
void pauseShell();
void forkAndExec(char **, struct execModifiers);
void checkForModifiers(struct execModifiers *, char **);
int openRedirection(struct execModifiers);
void processCommand(char **, struct execModifiers);
void closeRedirection(int savedStdin, int savedStdout);
void setParent();


#endif

/*
Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.
*/