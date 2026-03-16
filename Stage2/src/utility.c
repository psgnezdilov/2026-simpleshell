#include "simpleshell.h"

// Global variables for future use
static char *internalCommands[] = {
    "cd", "clr", "dir", "environ", "echo",
    "help", "pause", "quit", 0
};


// Attempt opening the file, if failed, exit the program
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

    while (token != NULL && i < MAX_ARGS - 1) {
        // MAX_ARGS - 1 to leave space for NULL
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
    // Go through the array with internal commands and check for matches
    for (int i = 0; internalCommands[i]; i++) {
        if (strcmp(command, internalCommands[i]) == 0) {
            return true; // match found, return true and exit function
        }
    }
    return false; // no matches
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
        // Check whether there's an argument for echo
        int i = 1;
        for (; args[i + 1] != NULL; i++) {
            // Go through args and output on one line
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
        while ((c = getchar()) != '\n' && c != EOF); // Flush the leftover buffer
    }
}

void help() {
    const char *envPath = getenv("SHELL");

    if (envPath == NULL) {
        fputs("Unable to get SHELL path\n", stderr);
    } else {
        char *envCopy = strdup(envPath);
        if (envCopy == NULL) {
            fputs("Unable to duplicate SHELL path\n", stderr);
        } else {
            // Find the length of both last two items of the path to remove and envCopy
            const int lastPathItemsLength = strlen("/bin/simpleshell");
            const int envCopyLength = strlen(envCopy);

            // Go to the end of the path and put a null terminator after Stage1/
            *(envCopy + envCopyLength - lastPathItemsLength) = '\0';

            // Assemble the full command and execute
            char fullCmd[PATH_MAX] = {0};
            snprintf(fullCmd, PATH_MAX, "more %s/manual/readme.txt", envCopy);
            system(fullCmd);

            // Free envCopy to avoid memory leaks in case of multiple uses
            free(envCopy);
            envCopy = NULL;
        }
    }
}

void cd(char **args) {
    // If program fails at any stage, throw a perror into stderr
    char currentDir[PATH_MAX] = {0};

    if (args[1] != NULL) {  // Check if a directory provided
        if (chdir(args[1]) != 0) {  // Try changing the directory
            perror("cd");
        } else {
            if (getcwd(currentDir, PATH_MAX) != NULL) { // Try getting the cwd
                setenv("PWD", currentDir, 1);
            } else {
                perror("cd -> getcwd failed after chdir");
            }
        }
    } else {  // No directory provided, try outputting the current one
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
        strncat(fullCommand, args[1], PATH_MAX - strlen(fullCommand) - 1); // -1 to account for the \0 character
    }
    system(fullCommand);
}

void env() {
    for (int i = 0; environ[i]; i++) {
        puts(environ[i]);
    }
}

void forkAndExec(char **args, struct execModifiers modifiers) {
    int returnCode;

    switch(fork()) {
        case -1: // Failed to fork
            fputs("Fork error\n", stderr);
            break;
        case 0: // Child process
            setParent(); // Sets PARENT variable
            execvp(args[0], args);
            perror("Exec error"); // If exec fails, we reach this command
            break;
        default: // Parent process
            if (modifiers.bgExec == false) {
                wait(&returnCode);
            }
    }
}

/*
The function goes through all the arguments and looks for execution modifiers.
As it progresses, it puts NULL pointers on the go not to modify argument array later for exec
*/
void checkForModifiers(struct execModifiers *modifiers, char **args) {
    modifiers -> bgExec = false;
    modifiers -> inFile = modifiers -> outFile = modifiers -> appendFile = NULL;
    for (int i = 0; args[i]; i++) {
        char *arg = args[i];
        if (strcmp(arg, "<") == 0) {
            modifiers -> inFile = args[i + 1];
            args[i++] = NULL;
        }
        else if (strcmp(arg, ">") == 0) {
            modifiers -> outFile = args[i + 1];
            args[i++] = NULL;
        }
        else if (strcmp(arg, ">>") == 0) {
            modifiers -> appendFile = args[i + 1];
            args[i++] = NULL;
        }
        else if (strcmp(arg, "&") == 0) {
            modifiers -> bgExec = true;
            args[i] = NULL;
        }
    }
}

// Open various files and redirect stdout/stdin streams depending on the presence of modifiers
void openRedirection(struct execModifiers modifiers) {
    if (modifiers.outFile != NULL) {
        if (freopen(modifiers.outFile, "w", stdout) == NULL) { // Open write redirection
            perror("Unable to proceed with '>' modifier");
            exit(EXIT_FAILURE);
        };
    } else if (modifiers.appendFile != NULL) {
        if (freopen(modifiers.appendFile, "a", stdout) == NULL) { // Open append redirection
            perror("Unable to proceed with '>>' modifier");
            exit(EXIT_FAILURE);
        }
    }
    if (modifiers.inFile != NULL) {
        if (freopen(modifiers.inFile, "r", stdin) == NULL) { // Open read redirection into command
            perror("Unable to proceed with '<' modifier");
            exit(EXIT_FAILURE);
        }
    }
}

void setParent() {
    char *parent = getenv("SHELL");
    if (parent == NULL) {
        fputs("Unable to get 'SHELL' variable from the environment\n", stderr);
        return;
    }
    setenv("PARENT", parent, 1);
}
/*
Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.
*/
