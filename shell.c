#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>

#define MAX_LINE 1024

int done = 0;
void ParseInput (char* input, char** args);

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */

        char *temp;
        char *input;
        size_t buffer_size; // unsigned long
        char *buffer = NULL;
        ssize_t read; // long
        read = getline(&buffer, &buffer_size, stdin);
        // printf("line: %s", buffer);
        
        // If input is length 0 or less.
        if(strlen(buffer) <= 0) 
        {
            return 0; // fix
        } 
        // Input has length 1 or greater
        else 
        {
            // Duplicate input
            input = strdup(buffer);

            // Parse input!
            ParseInput(input, args); //ParseInput (input string, args[] an array)

            // Check if History as been called '!!'
            if(strcmp(args[0], "!!")) {
                // RunHistoryCommand(); // TODO: Shuling
            }
            

            // Do we need to redirect? parameters (char ** args)
            // Do we need to pipe? (char ** args)
            // For loop through all of the args[] and strcmp(args[i], "|")
            for(int i = 0; args[i] != NULL; i++) {
                if(strcmp(args[i], "|")) {
                    // RunPipeCommand(); // TODO: Labib
                }
            }

            // For loop through all of the args[] and strcmp(args[i], "<")
            for(int i = 0; args[i] != NULL; i++) {
                if(strcmp(args[i], "<")) {
                    // RunInputCommand(); // TODO: Shuling
                }
            }
            // For loop through all of the args[] and strcmp(args[i], ">")
            for(int i = 0; args[i] != NULL; i++) {
                if(strcmp(args[i], ">")) {
                    // RunOutputCommand(); // TODO: Shuling
                }
            }

            // Execute command
            if(done == 0) {
                int rc = execvp(args[0], args);
            }


            // Check for 'exit'
            for(int i = 0; args[i] != NULL; i++) {
                if(strcmp(args[i], "exit")) {
                    // RunExitCommand(); // TODO: Labib
                }
            }
            

        }

        // duplicated from strdup
        free(input);
        
        // resized by getline
        free(buffer);

        should_run = 0;

    }
    return 0;
}


// Parse command!
void ParseInput (char* input, char** args) {
    int count = 0;
    char *pch;

    pch = strtok(input, " ");
    while(pch != NULL) {
        // printf("Token %d: %s\n", count, pch);
        args[count] = pch;
        ++count;

        pch = strtok(NULL, " ");
    }

    for(int i = 0; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    args[count] = NULL;

    int len = strlen(args[count-1]);
    args[count-1][len-1] = '\0';

    // Use tokenizeEx to help!! 
}