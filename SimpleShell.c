#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[], char*envp[]){
    pid_t child;
    char *command[15], *token, *lineptr = NULL;
    size_t i, n;
    int status;

    while(1){
        printf("praghaShell$$ ");
        if (getline(&lineptr, &n, stdin) == -1)
            break;
        token = strtok(lineptr, " \t\n\r");
        for (i=0; i<16 && token!= NULL; i++){
            command[i] = token;
            token = strtok(NULL, " \t\n\r");
        }
        command[i] = NULL;
        child =  fork();
        if(child == 0){
            if (execve(command[0], command, envp))
            {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        }

        if(child > 0){
            wait(&status);
        }
        
    }
    putchar('\n');
    free(lineptr);
    exit(status);
}
