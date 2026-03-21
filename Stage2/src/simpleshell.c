#include "simpleshell.h"

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    char prompt[PATH_MAX + 3]; // +3 to fit in the ": \0"
    FILE *inputPtr = stdin; // default

    // If argc > 1, then there's a batchfile provided
    if (argc > 1) {
        inputPtr = openFile(argv); // the program exits if unable to open the file
    }

    setShell(argv[0]); // Get shell path and set the environment

    cwdToPrompt(prompt); // Get the cwd path and format it for the prompt
    fputs(prompt, stdout);

    while (fgets(buffer, BUFFER_SIZE, inputPtr) != NULL) {
        struct execModifiers modifiers;
        char *args[MAX_ARGS] = {0};

        // Read and tokenize the input line
        char *bufCopy = strdup(buffer); // Free it at the end of the loop since it holds the strings
        tokenize(bufCopy, args); // Tokenize the "bufCopy" and save pointers to the "args"

        // Copy the state of the stdin and stdout before performing freopen
        int savedStdin = dup(STDIN_FILENO);
        int savedStdout = dup(STDOUT_FILENO);

        checkForModifiers(&modifiers, args); // Will replace modifiers with NULLs so that exec doesn't go past them

        processCommand(args, modifiers); // Perform checks whether internal or external, proceeds accordingly

        closeRedirection(savedStdin, savedStdout); // Redirects back to stdin and stdout, and closes saved FDs

        free(bufCopy); // Free the memory to avoid memory leak on next strdups
        bufCopy = NULL; // Just in case to avoid pointing to garbage

        cwdToPrompt(prompt); // Update the prompt after executing a command
        fputs(prompt, stdout);
        fflush(stdout); // Flush the prompt in case it gets stuck in the buffer after redirection
    }

    return 0;
}

/*
Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.
*/
