#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 128
#define MSG_SIZE 50
#define CODE_SIZE 10

void exeCommand(char *buffer, ssize_t command_size);
void writeReturn(char *msg, int code, int time_exe);

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
        //printf("%d\n", getpid()); // print child PID for testing: kill the process in another terminal
       //sleep(10); // to allow time to kill the process in another terminal
        sleep(2); // test for execution time measurement in seconds
        execlp(buffer, buffer, (char*)NULL);
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