#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#define PROMPT "==> "
#define SEPARATORS " \t\n"
#define BUFFER_SIZE 1024

/*
Function prototypes
    1. Set the shell path in the env
    2. Allocate memory for string
    3. Tokenize the buffer copy
    4. Check whether the command is internal or external
    5. Execute internal command
    6. echo helper
*/
char *allocateString(int);

void setShell();

void tokenize(char *, char **);

bool isInternal(const char *command);

void execInternal(char **args);

void echo(char **args);

void pauseShell();

void help();
// Static and global variables
static char *internalCommands[] = {
    "cd", "clr", "dir", "environ", "echo",
    "help", "pause", "quit", 0
};
extern char **environ;

int main(int argc, char *argv[]) {
    // Get shell path and set the environment
    setShell();
    // TODO: Prompt should include pathname of curr dir
    fputs(PROMPT, stdout);
    char buffer[BUFFER_SIZE] = {0};

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        char *args[64] = {0};
        // Read and tokenize the input line
        char *bufCopy = strdup(
            buffer); // Free it at the end of the loop since it holds the strings
        tokenize(bufCopy,
                 args); // Tokenize the "bufCopy" and save pointers to the "args"

        if (args[0] && isInternal(args[0])) {
            execInternal(args);
        } else if (args[0] == NULL) {
            fputs("No input, try again\n", stderr);
        } else {
            fprintf(stderr, "{%s} command is not supported by the shell\n", args[0]);
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
    setenv("SHELL", path, 1);

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
    args[i] = NULL;
}

bool isInternal(const char *command) {
    for (int i = 0; internalCommands[i]; i++) {
        if (strcmp(command, internalCommands[i]) == 0) {
            return true;
        }
    }
    return false;
}

void execInternal(char **args) {
    const char *cmd = args[0];
    if (strcmp(cmd, "cd") == 0) {
        // TODO: Maybe move part of the logic into helper functions
        char currentDir[PATH_MAX] = {0};

        if (args[1] != NULL) {
            if (chdir(args[1]) != 0) {
                perror("cd");
            } else {
                if (getcwd(currentDir, PATH_MAX) != NULL) {
                    setenv("PWD", currentDir, 1);
                } else {
                    perror("cd -> getcwd failed after chdir");
                }
            }
        } else {
            if (getcwd(currentDir, PATH_MAX) != NULL) {
                puts(currentDir);
            } else {
                perror("cd -> getcwd failed");
            }
        }
    } else if (strcmp(cmd, "clr") == 0) {
        system("clear");
    } else if (strcmp(cmd, "dir") == 0) {
        char fullCommand[PATH_MAX] = "ls -al ";
        if (args[1]) {
            strncat(fullCommand, args[1], PATH_MAX - strlen(fullCommand) - 1);
        }
        system(fullCommand);
    } else if (strcmp(cmd, "environ") == 0) {
        for (int i = 0; environ[i]; i++) {
            puts(environ[i]);
        }
    } else if (strcmp(cmd, "echo") == 0) {
        echo(args);
    } else if (strcmp(cmd, "help") == 0) {
        help();
    } else if (strcmp(cmd, "pause") == 0) {
        pauseShell();
    } else if (strcmp(cmd, "quit") == 0) {
        exit(0);
    }
}

void echo(char **args) {
    if (args[1] != NULL) {
        int i = 1;
        for (; args[i + 1] != NULL; i++) {
            printf("%s ", args[i]);
        }
        fputs(args[i], stdout);
    }
    fputs("\n", stdout);
}

void pauseShell() {
    int firstChar;
    // Check whether the first char of the line is the \n or EOF, if not, flush the input buffer and wait for the next
    while ((firstChar = getchar()) != '\n' && firstChar != EOF) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF); // Flush the leftover buffer
    }
}

// TODO: Add error handling
void help() {
    const char *envPath = getenv("SHELL");
    char *envCopy = strdup(envPath);

    char *lastSlash = strrchr(envCopy, '/');
    if (lastSlash != NULL) {
        *(lastSlash + 1) = '\0';
    } else {
        envCopy[0] = '\0';
    }

    char fullCmd[PATH_MAX] = {0};
    snprintf(fullCmd, PATH_MAX, "more %smanual/readme.txt", envCopy);
    system(fullCmd);

    free(envCopy);
    envCopy = NULL;
}