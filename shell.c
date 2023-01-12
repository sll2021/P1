#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>

#define MAX_LINE 1024

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("osh> ");
        fflush(stdout);
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
        
        /* Start of Copy GETTING ARGUMENTS FROM CMD*/

        char *temp;
        char *input;
        size_t buffer_size; // unsigned long
        char *buffer = NULL;
        ssize_t read; // long
        read = getline(&buffer, &buffer_size, stdin);
        printf("line: %s", buffer);
        
        // If input is length 0 or less.
        if(strlen(buffer) <= 0) 
        {
            return 0;
        } 
        // Input has length 1 or greater
        else 
        {
            // Duplicate command
            input = strdup(buffer);

            // Parse command!
            ParseInput(input, args); //ParseInput (input string, args[] an array)

            // Check if History as been called '!!'

            // Do we need to redirect? parameters (char ** args)
            // Do we need to pipe? (char ** args)
            // TODO: For loop through all of the args[] and strcmp(args[i], "|")
            // TODO: For loop through all of the args[] and strcmp(args[i], "<")
            // TODO: For loop through all of the args[] and strcmp(args[i], ">")


        }

        

        
        // duplicated from strdup
        free(input);
        
        // resized by getline
        free(buffer);

        /* End of Copy GETTING ARGUMENTS FROM CMD*/

        should_run = 0;

    }
    return 0;
}


// TODO: Parse command!
void ParseCommand (char* input, char** args) {
    
    // Use tokenizeEx to help!!
}