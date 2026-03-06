#include "simpleshell.h"

// TODO: Potentially reduce overhead by creating a boolean var that tracks whether the dir has been changed in cd func
int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    char prompt[PATH_MAX + 3]; // +3 to fit in the ": \0"
    FILE *inputPtr = stdin; // default

    if (argc > 1) {
        inputPtr = openFile(argv); // the program exits if unable to open the file
    }

    cwdToPrompt(prompt);       // Get the cwd path and format it for the prompt
    setShell();                // Get shell path and set the environment
    fputs(prompt, stdout);

    while (fgets(buffer, BUFFER_SIZE, inputPtr) != NULL) {
        char *args[MAX_ARGS] = {0};
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

        free(bufCopy);            // Free the memory to avoid memory leak on next strdups
        bufCopy = NULL;           // Just in case to avoid pointing to garbage
        cwdToPrompt(prompt);      // Update the prompt after executing a command
        fputs(prompt, stdout);
    }

    return 0;
}