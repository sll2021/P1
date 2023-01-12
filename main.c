#include <stdlib.h>  //exit
#include <stdio.h>   //perror
#include <unistd.h>  //fork, pipe
#include <sys/wait.h>   //wait

int main() {
    enum { READ, WRITE };
    pid_t pid;
    int pipeFD[2];

    if(pipe(pipeFD) < 0) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid < 0) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    if(pid == 0) {
        close(pipeFD[READ]);
        dup2(pipeFD[WRITE], STDOUT_FILENO);

        int rc = execlp("/bin/ls", "ls", "-l", (char *) 0); //(char*) 0 can be NULL

        printf("Never getting here!");
        sleep(5);

    } else {
        close(pipeFD[WRITE]);


        wait (NULL); //Wait for the child to finish!!
        char buf[4096];
        int n = read(pipeFD[READ], buf, 4096);
        buf[n] = '\0';
        for (int i = 0; i < n; ++i)
        printf("%c ", buf[i]);

        printf("I am the parent\n");

        //print out

    }

    return 0;
}