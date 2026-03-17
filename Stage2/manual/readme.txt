The shell supports internal, external commands, redirection, and background execution.

INTERNAL COMMANDS:

    cd [DIRECTORY_PATH: Optional] => Change directory to the provided path, if no path is provided
                                            display current directory

    clr => Clear the terminal

    dir [DIRECTORY_PATH: Optional] => Display all the files in the provided directory, if no directory is provided
                                            display all the files in the current directory

    environ => Display all the environment variables

    echo [INPUT] => Echo the input followed by a new line

    help => Display user manual

    pause => Pause shell operation until 'ENTER' is pressed

    quit => Quit the shell

EXTERNAL COMMANDS:

    External commands are all the commands that can be found in directories included in the PATH and that are not
    internal.

    EXAMPLE:
        grep, sort, etc.

REDIRECTION AND BACKGROUND EXECUTION:

    The shell supports redirection from and to a file.

    1. Redirection to a file ">", ">>":
    
        Available only for both internal and external commands, allows to redirect the output to a specified file in two ways.

        Write redirection ">" and append redirection ">>". Write redirection will rewrite everything in the file. In turn,
        append redirection will only add to the end of the file the output.

        If the specified do not exist, it will be created automatically in both cases.
        
        USAGE:
            1. command [args] > file
            2. command [args] >> file

        EXAMPLES:
            1. ls -laR > test.txt (replaces everything in the file if there is something already)

            2. ls -laR >> test.txt (add to the end of the file)

    2. Redirection from a file "<":

        Available only for external commands, the command will be taking arguments from a specified file instead of stdin.

        USAGE:
            externalCommand [args] < file

        EXAMPLE:
            sort < test.txt (will sort the contents of the file and output to the stdout)

    BACKGROUND EXECUTION "&":

    The shell also supports background exectuion only for external commands.

    If "&" is present in the input, the command will be run in the background instead of a foreground. This means that
    while the called command with "&" is processing the command e.g. "ls -laR /", another command can be run without waiting for
    the previous one to finish.

        EXAMPLE:
        ls -laR / & (run recursive ls in the background)
        ls (call ls on the current directory while the previous one is still in execution)

    USING REDIRECTION AND BACKGROUND EXECUTION AT THE SAME TIME:

        The redirection and background execution can be used together as a part of the same prompt.

        EXAMPLE:

            1. ls -laR / > test.txt & (Recursively list all the files starting from root to the test.txt. Execute in the background)

            2. sort < test.txt >> sorted.txt & (Sort the contents of the test.txt, append to the sorted.txt. Execute everything in the background)

Name: Pavel Soshenko Gnezdilov
Student Number: 41071
I acknowledge the DCU Academic Integrity Policy.