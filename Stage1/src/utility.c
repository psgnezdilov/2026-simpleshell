#include "simpleshell.h"

static char *internalCommands[] = {"cd",   "clr",   "dir",  "environ", "echo",
                                   "help", "pause", "quit", 0};

FILE *openFile(char **argv) {
  FILE *filePtr = fopen(argv[1], "r");
  if (filePtr == NULL) {
    fprintf(stderr, "Unable to open the file \"%s\"\n", argv[1]);
    exit(1);
  }
  return filePtr;
}

char *allocateString(const int size) {
  char *string = calloc(size, sizeof(char));
  if (!string) {
    fputs("Unable to allocate memory for string\n", stderr);
    exit(1);
  }
  return string;
}

// TODO: Fix finding the shell path if not executed directly from the bin
// directory
void setShell(char *relativePath) {
  char *path = allocateString(BUFFER_SIZE);
  if (realpath(relativePath, path) == NULL) {
    fputs("Unable to resolve shell path\n", stderr);
  }
  setenv("SHELL", path, 1);

  free(path);
  path = NULL;
}

void tokenize(char *buffer, char **args) {
  char *saveptr = NULL;
  char *token = strtok_r(buffer, SEPARATORS, &saveptr);
  int i = 0;

  while (token != NULL &&
         i < MAX_ARGS - 1) { // MAX_ARGS - 1 to leave space for NULL
    args[i] = token;
    token = strtok_r(NULL, SEPARATORS, &saveptr);
    i++;
  }
  args[i] = NULL;
}

void cwdToPrompt(char *cwd) {
  if (getcwd(cwd, PATH_MAX) != NULL) {
    strcat(cwd, ": ");
  } else {
    fputs("Unable to get cwd\n", stderr);
    exit(1); // Design choice to end the program if we don't get the cwd since
             // the prompt will be broken Could have a default instead, but for
             // now we just exit the program
  }
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
    cd(args);
  } else if (strcmp(cmd, "clr") == 0) {
    system("clear");
  } else if (strcmp(cmd, "dir") == 0) {
    dir(args);
  } else if (strcmp(cmd, "environ") == 0) {
    env();
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
  fputs("Press ENTER to resume\n", stdout);
  int firstChar;
  // Check whether the first char of the line is the \n or EOF, if not, flush
  // the input buffer and wait for the next
  while ((firstChar = getchar()) != '\n' && firstChar != EOF) {
    fputs("Press ENTER to resume\n", stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
      ; // Flush the leftover buffer
  }
}

// TODO: Fix finding the manual path if not executed directly from the bin
// directory, related to setShell();
void help() {
  const char *envPath = getenv("SHELL");

  if (envPath == NULL) {
    fputs("Unable to get SHELL path\n", stderr);
  } else {
    char *envCopy = strdup(envPath);
    if (envCopy == NULL) {
      fputs("Unable to duplicate SHELL path\n", stderr);
    } else {
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
  }
}

void cd(char **args) {
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
}

void dir(char **args) {
  char fullCommand[PATH_MAX] = "ls -al ";
  if (args[1]) {
    strncat(fullCommand, args[1], PATH_MAX - strlen(fullCommand) - 1);
  }
  system(fullCommand);
}

void env() {
  for (int i = 0; environ[i]; i++) {
    puts(environ[i]);
  }
}
