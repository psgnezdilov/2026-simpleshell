#include "simpleshell.h"

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

    // FIXME: Not allow and <
    // FIXME: Handle help redirection to a file
    while (fgets(buffer, BUFFER_SIZE, inputPtr) != NULL) {
        struct execModifiers modifiers;
        char *args[MAX_ARGS] = {0};

        // Read and tokenize the input line
        char *bufCopy = strdup(buffer); // Free it at the end of the loop since it holds the strings
        tokenize(bufCopy, args); // Tokenize the "bufCopy" and save pointers to the "args"

        checkForModifiers(&modifiers, args); // Will replace modifiers with NULLs so that exec doesn't go past them
        openRedirection(modifiers); // redirects streams in/from files before exec

        if (args[0] && isInternal(args[0])) {
            execInternal(args);
        } else if (args[0] == NULL) {
            fputs("No input, try again\n", stderr);
        } else {
            forkAndExec(args, modifiers);
        }

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
