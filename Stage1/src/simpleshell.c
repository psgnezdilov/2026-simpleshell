#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROMPT "==> "
#define SEPARATORS " \t\n"
#define BUFFER_SIZE 1024

/*
Function prototypes
    1. Set the shell path in the env
    2. Allocate memory for string
    3. Tokenize the buffer copy
    4. Check whether the command is internal or external
*/
char *allocateString(int);

void setShell();

void tokenize(char *, char **);

bool isInternal(const char *command);

void execInternal(char **args);

// Static and global variables
static char *internalCommands[] = {
    "cd", "clr", "dir", "environ", "echo",
    "help", "pause", "quit", 0
};
extern char **environ;

int main(int argc, char *argv[]) {
    // Get shell path and set the environment
    setShell();

    fputs(PROMPT, stdout);
    char buffer[BUFFER_SIZE] = {0}; // TODO: On each fgets loop remove garbage from the buffer

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        char *args[64];
        // Read and tokenize the input line
        char *bufCopy = strdup(
            buffer); // Free it at the end of the loop since it holds the strings
        tokenize(bufCopy,
                 args); // Tokenize the "bufCopy" and save pointers to the "args"

        if (isInternal(args[0])) {
            execInternal(args);
        }

        free(bufCopy);
        bufCopy = NULL;
        fputs(PROMPT, stdout);
    }
}

char *allocateString(const int size) {
    char *string = calloc(size, sizeof(char));
    if (!string) {
        fputs("Unable to allocate memory for string\n", stderr);
        exit(1);
    }
    return string;
}

void setShell() {
    char *path = allocateString(BUFFER_SIZE);
    getcwd(path, BUFFER_SIZE);
    setenv("shell", path, 1);

    free(path);
    path = NULL;
}

void tokenize(char *buffer, char **args) {
    char *saveptr = NULL;
    char *token = strtok_r(buffer, SEPARATORS, &saveptr);
    int i = 0;

    while (token != NULL) {
        args[i] = token;
        token = strtok_r(NULL, SEPARATORS, &saveptr);
        i++;
    }
}

bool isInternal(const char *command) {
    bool internal = false;
    for (int i = 0; internalCommands[i]; i++) {
        if (strcmp(command, internalCommands[i]) == 0) {
            internal = true;
            break;
        }
    }

    return internal;
}

void execInternal(char **args) {
    const char *cmd = args[0];
    if (strcmp(cmd, "cd") == 0) {
        char currentDir[BUFFER_SIZE] = {0};

        if (args[1] != NULL) {
            if (chdir(args[1]) != 0) {
                perror("cd");
            } else {
                if (getcwd(currentDir, BUFFER_SIZE) != NULL) {
                    setenv("PWD", currentDir, 1);
                } else {
                    perror("cd -> getcwd failed after chdir");
                }
            }
        } else {
            if (getcwd(currentDir, BUFFER_SIZE) != NULL) {
                puts(currentDir);
            } else {
                perror("cd -> getcwd failed");
            }
        }
    } else if (strcmp(cmd, "clr") == 0) {
        system("clear");
    } else if (strcmp(cmd, "dir") == 0) {
        char fullCommand[BUFFER_SIZE] = "ls -al ";
        if (args[1]) {
            strncat(fullCommand, args[1], BUFFER_SIZE - strlen(cmd));
        }
        system(fullCommand);
    } else if (strcmp(cmd, "environ") == 0) {
        for (int i = 0; environ[i]; i++) {
            puts(environ[i]);
        }
    } else if (strcmp(cmd, "echo") == 0) {
    } else if (strcmp(cmd, "help") == 0) {
    } else if (strcmp(cmd, "pause") == 0) {
    } else if (strcmp(cmd, "quit") == 0) {

    }
}
