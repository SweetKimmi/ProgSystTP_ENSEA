#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFFER_SIZE 128

int main(int argc, char *argv[]) {
    // Welcome message
    const char* welcome_msg = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
    write(STDOUT_FILENO, welcome_msg, strlen(welcome_msg));

   // Prompt
    const char* prompt = "\nenseash % ";

    char buffer[BUFFER_SIZE];
    ssize_t command_size;

    while (1){
        // Prompt
        write(STDOUT_FILENO, prompt, strlen(prompt)); 

        // Read
        command_size = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        buffer[command_size-1] = '\0'; // replace newline with null terminator

        // Excecution of commands
        exeCommand(buffer, command_size);
    }
}

  
void exeCommand(char *buffer, ssize_t command_size) {

    // Shell exit
    if (strcmp("exit", buffer) == 0 || command_size == 0) {
        write(STDOUT_FILENO, "Exit shell...\n", strlen("Exit shell...\n"));
        exit(EXIT_FAILURE);
    }
    int pid, status;
    pid = fork();
    
    if (pid != 0){ // Parent process
        wait(&status);
    }
    else {
        execlp(buffer, buffer, (char*)NULL);
        exit(EXIT_SUCCESS);
    }
}    

