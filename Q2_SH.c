#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    // Fortune
    if (strcmp(buffer, "fortune") == 0){
        int pid, statuts;
        pid = fork();

        if (pid!=0){ // Parent process
            wait(&statuts);
            }
        else {
            execlp("fortune","fortune",(char*)NULL);
            const char* fortune_msg= "Today is what happened to yesterday.";
            write(STDOUT_FILENO, fortune_msg, strlen(fortune_msg));
           }
        }

   // Date
    else if (strcmp(buffer, "date") == 0){
        int pid, statuts;
        pid = fork();

        if (pid!=0){
            wait(&statuts);
        }
        else {
            execlp("date","date",(char*)NULL);
        }
    }

    // Display
    else {
        write(STDOUT_FILENO, buffer, command_size);
    }
    return 0;
}
   

