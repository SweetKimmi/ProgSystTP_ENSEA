#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFFER_SIZE 128
#define MSG_SIZE 50
#define CODE_SIZE 10

int main(int argc, char *argv[]) {
    // Welcome message
    const char* welcome_msg = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
    write(STDOUT_FILENO, welcome_msg, strlen(welcome_msg));

   // Prompt
    const char* prompt = "\nenseash % ";
    write(STDOUT_FILENO, prompt, strlen(prompt)); 

    char buffer[BUFFER_SIZE];
    ssize_t command_size;

    while (1){
        // Prompt
        //write(STDOUT_FILENO, prompt, strlen(prompt)); 

        // Read
        command_size = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        buffer[command_size-1] = '\0'; // replace newline with null terminator to get the command string only

        // Excecution of commands
        exeCommand(buffer, command_size);
    }
}

  
void exeCommand(char *buffer, ssize_t command_size) {

    // Shell exit
    if (strcmp("exit", buffer) == 0 || command_size == 0) { // exit on "exit" command or ctrl+D
        write(STDOUT_FILENO, "Exit shell...\n", strlen("Exit shell...\n"));
        exit(EXIT_FAILURE);
    }

    int pid, status;
    pid = fork();
    
    if (pid != 0){ // Parent process waits for child to finish
        wait(&status);
        char msg_out[MSG_SIZE];

        if (WIFEXITED(status)) {
            strcpy(msg_out, "enseash [exit:");
            writeReturn(msg_out,WEXITSTATUS(status));// WEXITSTATUS gets the exit status of the child process
        }
        else if (WIFSIGNALED(status)) { 
            strcpy(msg_out, "enseash [sign:");
            writeReturn(msg_out, WTERMSIG(status));// WTERMSIG gets the signal number that caused the termination
        }
    }
    else { // Child process executes command
        printf("%d\n", getpid()); // print child PID for testing: kill the process in another terminal
        sleep(10); // to allow time to kill the process in another terminal
        execlp(buffer, buffer, (char*)NULL);
        exit(EXIT_FAILURE);
    }
}  


void writeReturn(char *msg, int code) {
    char code_str[CODE_SIZE];
    sprintf(code_str, "%d", code);
    strcat(msg, code_str);
    strcat(msg, "] % ");
    write(STDOUT_FILENO, msg, strlen(msg));
}