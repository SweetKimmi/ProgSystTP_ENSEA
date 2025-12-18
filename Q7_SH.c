#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 128
#define MSG_SIZE 50
#define CODE_SIZE 10
#define ARG_SIZE 20

void exeCommand(char **buffer, ssize_t command_size);
void writeReturn(char *msg, int code);
char **splitcommand(char *cmd);

int main(int argc, char *argv[]) {
    // Welcome message
    const char* welcome_msg = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
    write(STDOUT_FILENO, welcome_msg, strlen(welcome_msg));

   // Prompt
    const char* prompt = "\nenseash % ";

    char buffer[BUFFER_SIZE];
    ssize_t command_size;
    char **list_cmd; 

    while (1){
        // Prompt
        write(STDOUT_FILENO, prompt, strlen(prompt)); 

        memset(buffer, 0, BUFFER_SIZE); //clear buffer

        // Read
        if ((command_size = read(0, buffer, BUFFER_SIZE)) == -1) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        list_cmd = splitcommand(buffer);
    
        // Excecution of commands
        exeCommand(list_cmd, command_size);
    }
}

  
void exeCommand(char **buffer, ssize_t command_size) { //modified to take an array of strings

    // Shell exit
    if (buffer[0] != NULL && strcmp("exit", buffer[0]) == 0) {
        write(STDOUT_FILENO, "Exit shell...\n", strlen("Exit shell...\n"));
        exit(EXIT_FAILURE);
    }

    int pid, status;
    pid = fork();
    
    if (pid != 0){ // Parent process waits for child to finish
        wait(&status);
    }
    else { // Child process executes command

        int file;
        for (int i =0; buffer[i] != NULL; i++){

            if (strcmp(buffer[i], ">") == 0 ) {
                file = open(buffer[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (file == -1) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                if (dup2(file, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                buffer[i] = NULL;
                break;
            }
            else if (strcmp(buffer[i], "<") == 0) {
                file = open(buffer[i + 1], O_RDONLY);
                if (file == -1) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                if (dup2(file, STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                buffer[i] = NULL; 
                break;
            }
            
        }
        close(file);
        execvp(buffer[0], buffer ); // modified to use the array of strings
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

char** splitcommand(char *cmd) {
    // Function to split command string into array of strings
    char **argv ;
    argv = malloc(ARG_SIZE*sizeof(char*)); 
    int i = 0;

    char *tmp = strtok(cmd, " "); // split command into tokens based on space delimiter

    while (tmp != NULL) {
        argv[i] = malloc(ARG_SIZE*sizeof(char*));
        strcpy(argv[i], tmp);
        tmp = strtok(NULL, " "); // get next token
        i++;
    }

    argv[i -1][strlen(argv[i -1]) - 1 ] = '\0'; // null terminate the last argument
    return argv;
}