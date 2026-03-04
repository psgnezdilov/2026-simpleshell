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
static char *internalCommands[] = {"cd",   "clr",   "dir",  "environ", "echo",
                                   "help", "pause", "quit", 0};
extern char **environ;

int main(int argc, char *argv[]) {

  // Get shell path and set the environment
  setShell();

  fputs(PROMPT, stdout);
  char buffer[BUFFER_SIZE] = {0};

  while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
    char *args[64];
    // Read and tokenize the input line
    char *bufCopy = strdup(
        buffer); // Free it at the end of the loop since it holds the strings
    tokenize(bufCopy,
             args); // Tokenize the "bufCopy" and save pointers to the "args"

    free(bufCopy);
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
