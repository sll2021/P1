#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <sys/stat.h> //added
#include <fcntl.h> //added

#define MAX_LINE 1024

int done = 0;
pid_t pid; 
int should_wait = 1;
char *history_buffer; //new
char *history_args[MAX_LINE/2 + 1]; //new
void ParseInput (char* input, char** args);
void RunPipeCommand(char** args);
void RunHistoryCommand(char* input);
void RunInputCommand(char** args, int file_index);
void RunOutputCommand(char** args, int file_index);


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

            // Check for 'exit'
            for(int i = 0; args[i] != NULL; i++) {
                if(strcmp(args[i], "exit") == 0) {
                    // RunExitCommand();
                    done = 1;
                    wait(NULL);
                    should_run = 0;
                }
            }

            pid = fork();

            if(pid == -1) {
                printf("Forking failed");
            }
            else if (pid == 0) {
                // Check if History as been called '!!'
                if(strcmp(args[0], "!!")  == 0) {
                    RunHistoryCommand(input); // TODO: Shuling
                }
                

                // Do we need to redirect? parameters (char ** args)
                // Do we need to pipe? (char ** args)
                // For loop through all of the args[] and strcmp(args[i], "|")
                for(int i = 0; args[i] != NULL; i++) {
                    if(strcmp(args[i], "|")  == 0) {
                        RunPipeCommand(args);
                        // RunPipeCommand();
                    }
                }

                // For loop through all of the args[] and strcmp(args[i], "<")
                for(int i = 0; args[i] != NULL; i++) {
                    if(strcmp(args[i], "<")  == 0) {
                        //RunInputCommand(args, (i+1)); // TODO: Shuling
                    }
                }
                // For loop through all of the args[] and strcmp(args[i], ">")
                for(int i = 0; args[i] != NULL; i++) {
                    if(strcmp(args[i], ">") == 0) {
                        // RunOutputCommand(); // TODO: Shuling
                    }
                }

                

                // Execute command
                if(done == 0) {
                    int rc = execvp(args[0], args);
                }
            } 
            else {
                if(should_wait == 1) {
                    wait(NULL);
                }
            }

        }

        // duplicated from strdup
        free(input);
        
        // resized by getline
        free(buffer);

        

    }
    return 0;
}


// Parse command!
void ParseInput (char* input, char** args) { 
    int count = 0;
    char *pch;

    pch = strtok(input, " ");
    while(pch != NULL) {
        if(strcmp(pch, "&") == 0) { // TODO: Deals with &. Not sure if this totally works
            should_wait = 0;
            args[count] = "\0";
            break;
        }


        args[count] = pch;
        ++count;

        
        pch = strtok(NULL, " ");
    }

    // for(int i = 0; args[i] != NULL; i++) {
    //     printf("%s ", args[i]);
    // }
    args[count] = NULL;

    int len = strlen(args[count-1]);
    args[count-1][len-1] = '\0';

    // Use tokenizeEx to help!! 
}

void RunPipeCommand(char** args) {
    // Seperating the two commands into two arrays
    int index = 0; // Location of the pipe

    // Find where the pipe's index is
    for(int i = 0; args[i] != NULL; i++) {
        if(strcmp(args[i],"|") == 0) {
            index = i;
        }
    }

    char *first[MAX_LINE/2 + 1];
    char *second[MAX_LINE/2 + 1];
    for(int i = 0; i < index; i++) {
        first[i] = args[i];
    }
    first[index] = NULL;

    for(int i = index + 1; args[i] != NULL; i++) {
        second[i - 1 - index] = args[i];
        second[i - index] = NULL;
    }
    
    // Run commands
    pid_t pid;
    int pipeFD[2];

    if(pipe(pipeFD) < 0) 
    {
        perror("Error during pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid < 0) {
        perror("Error during fork in pipe!");
        exit(EXIT_FAILURE);
    }

    else if(pid == 0) 
    {
        close(pipeFD[0]); // close stdin
        dup2(pipeFD[1], 1); // dup stdout

        int rc = execvp(first[0], first);
        if(rc < 0) {
            printf("Error in running Command");
        }        
    }
    else if (pid > 0) 
    {
        wait(NULL);
        close(pipeFD[1]); // close stdout
        dup2(pipeFD[0], 0); // dup stdin

        int rc = execvp(second[0], second);
        if(rc < 0) {
            printf("Error in running Command");
        }     
    }
}

//new
void RunHistoryCommand(char* input)
{       
    if(history_buffer == NULL)
    {
        printf("No commands in history.");
        return;
    }
    
    else
    {
        //int history_status = execvp(history_args[0], history_args); 
        printf("The most recent command in history is: \n"); //to be edited
        printf("%s\n", history_buffer);
        strcpy(input, history_buffer);
        strcpy(history_buffer, input); //place new command in history buffer
        ParseInput(history_buffer, history_args); //place new command in history argument list
    }
}

//new <
void RunInputCommand(char** args, int file_index)
{
    FILE *fp;
    char path[1035];

    int fd = open(args[file_index], O_RDONLY); //get a file descriptor for the file
    
    if(fd == -1)
    {
        printf("Unable to open the file.");
        return;
    }

    dup2(fd, STDIN_FILENO);
    close(fd);
}

//new >
void RunOutputCommand(char** args, int file_index)
{   
    FILE *fp;
    char path[1035];

    int fd = open(args[file_index], O_RDWR); //get a file descriptor for the file
    
    if(fd == -1)
    {
        printf("Unable to open the file.");
        return;
    }

    for(int i = 0; args[i] != ">"; i++) //store the command to be executed in path 
    {   
        if(i == 0)
        {
            strcpy(path, args[i]);
        }
        else
        {
            strcat(path, args[i]);
        }
        strcat(path, " ");
    }

    fp = popen(path, "r"); //execute the command
    if(fp == NULL) //handle error message
    {
        printf("Command is invalid.");
        return;
    }

    //read and print from the output of command line by line
    while(fgets(path, sizeof(path), fp) != NULL) 
    {
        fprintf(stdout, path);
    }

    dup2(fd, STDOUT_FILENO); // redirect the output from standard output to .txt file
    close(fd);
    pclose(fp);
}