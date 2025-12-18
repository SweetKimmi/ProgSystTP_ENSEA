#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 128
#define MSG_SIZE 50
#define CODE_SIZE 10
#define ARG_SIZE 20

void exeCommand(char **buffer, ssize_t command_size);
void writeReturn(char *msg, int code, int time_exe);
char **splitcommand(char *cmd);

int main(int argc, char *argv[]) {
    // Welcome message
    const char* welcome_msg = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
    write(STDOUT_FILENO, welcome_msg, strlen(welcome_msg));

   // Prompt
    const char* prompt = "\nenseash % ";
    write(STDOUT_FILENO, prompt, strlen(prompt));

    char buffer[BUFFER_SIZE];
    ssize_t command_size;
    char **list_cmd; // array of strings to hold command and arguments

    while (1){
        // Prompt
        //write(STDOUT_FILENO, prompt, strlen(prompt)); 

        memset(buffer, 0, BUFFER_SIZE); //clear buffer

        // Read
        if ((command_size = read(0, buffer, BUFFER_SIZE)) == -1) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        list_cmd = splitcommand(buffer); // split command into array of strings
    
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
    struct timespec start, stop;
    start.tv_nsec = 0;
    stop.tv_nsec = 0;
    start.tv_sec = 0;
    stop.tv_sec = 0;
    long time_exe_ns = 0;
    long time_exe_sec = 0;
    int time_exe = 0;
    
    if (pid != 0){ // Parent process waits for child to finish
         if (clock_gettime(CLOCK_REALTIME, &start) == -1) { // get start time
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        wait(&status);
        char msg_out[MSG_SIZE];

        if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { // get end time
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        time_exe_ns = stop.tv_nsec - start.tv_nsec; // calculate execution time part in ns
        time_exe_sec = stop.tv_sec - start.tv_sec; // calculate execution time part in s
        time_exe = time_exe_sec * 1000 + time_exe_ns / 1000000; // total time in ms; 

        if (WIFEXITED(status)) {
            strcpy(msg_out, "enseash [exit:");
            writeReturn(msg_out,WEXITSTATUS(status), time_exe);// WEXITSTATUS gets the exit status of the child process
        }
        else if (WIFSIGNALED(status)) { 
            strcpy(msg_out, "enseash [sign:");
            writeReturn(msg_out, WTERMSIG(status), time_exe);// WTERMSIG gets the signal number that caused the termination
        }
    }
     else { // Child process executes command

        int file; // file descriptor for redirection
        for (int i =0; buffer[i] != NULL; i++){ // check for redirection operators

            if (strcmp(buffer[i], ">") == 0 ) { // output redirection
                file = open(buffer[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file for writing, create if it doesn't exist, truncate if it does

                if (file == -1) { // error opening file
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                if (dup2(file, STDOUT_FILENO) == -1) { // redirect stdout to file
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                buffer[i] = NULL; // null terminate the command before the redirection operator 
                break; // exit loop after handling redirection
            }
            else if (strcmp(buffer[i], "<") == 0) { // input redirection
                file = open(buffer[i + 1], O_RDONLY); // open file for reading
                if (file == -1) { 
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                if (dup2(file, STDIN_FILENO) == -1) { // redirect stdin to file
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


void writeReturn(char *msg, int code, int time_exe) {
    char code_str[CODE_SIZE];
    sprintf(code_str, "%d", code); // convert int to string
    strcat(msg, code_str); // concatenate code to message
    strcat(msg,"|");
    sprintf(code_str, "%d", time_exe); //convert time to string
    strcat(msg, code_str); // concatenate time to message
    strcat(msg, "ms] % "); 
    write(STDOUT_FILENO, msg, strlen(msg));
}


char** splitcommand(char *cmd) {
    // Function to split command string into array of strings
    char **argv ; // array of strings
    argv = malloc(ARG_SIZE*sizeof(char*)); // allocate memory for array of strings
    int i = 0; 

    char *tmp = strtok(cmd, " "); // split command into tokens based on space delimiter

    while (tmp != NULL) { // while there are tokens
        argv[i] = malloc(ARG_SIZE*sizeof(char*)); // allocate memory for each argument
        strcpy(argv[i], tmp); // copy token to argument array
        tmp = strtok(NULL, " "); // get next token
        i++;
    }

    argv[i -1][strlen(argv[i -1]) - 1 ] = '\0'; // null terminate the last argument
    return argv;
}