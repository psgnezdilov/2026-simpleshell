#include "simpleshell.h"

// TODO: Fork and execute external commands
// TODO: Accept stdin and stdout as arguments as well as both '>' and '>>'
// TODO: Background execution

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    char prompt[PATH_MAX + 3]; // +3 to fit in the ": \0"
    FILE *inputPtr = stdin; // default

    // If argc > 1, then there's a batchfile provided
    if (argc > 1) {
        inputPtr = openFile(argv); // the program exits if unable to open the file
    }

    cwdToPrompt(prompt); // Get the cwd path and format it for the prompt
    setShell(argv[0]); // Get shell path and set the environment
    fputs(prompt, stdout);

    while (fgets(buffer, BUFFER_SIZE, inputPtr) != NULL) {
        char *args[MAX_ARGS] = {0};
        // Read and tokenize the input line
        char *bufCopy = strdup(buffer); // Free it at the end of the loop since it holds the strings
        tokenize(bufCopy, args); // Tokenize the "bufCopy" and save pointers to the "args"

        if (args[0] && isInternal(args[0])) {
            execInternal(args);
        } else if (args[0] == NULL) {
            fputs("No input, try again\n", stderr);
        } else {
            forkAndExec(args);
        }

        // FIXME: Fix the output after fork and exec
        free(bufCopy); // Free the memory to avoid memory leak on next strdups
        bufCopy = NULL; // Just in case to avoid pointing to garbage
        cwdToPrompt(prompt); // Update the prompt after executing a command
        fputs(prompt, stdout);
    }

    return 0;
}

/*
Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.
*/
