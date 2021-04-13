#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>


#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

/**
 * Author: Jacob Gerega
 * Pledge: I pledge my honor that I have abided by the Stevens Honor System.
 ***/

volatile sig_atomic_t signal_value = 0;

void catch_signal(int sig){
    signal_value = sig;
}

int main(int argc, char* argv[]){
    while(true){
        char cwd[PATH_MAX]; //current working directory
        printf("[%s%s%s]$ ", BRIGHTBLUE, getcwd(cwd, PATH_MAX), DEFAULT); //print curr directory
        char line[PATH_MAX]; //line input
        fgets(line, PATH_MAX, stdin); //read in line input
        line[strlen(line)-1] = '\0';

        char* token = strtok(line, " \t");
        char* tokens[PATH_MAX];
        int tc = 0;
        tokens[0] = token;
        while(token != NULL){
            tokens[tc] = token;
            tc++;
            token = strtok(NULL, " \t");
        }
        if(tokens[0] == NULL){
            continue;
        }
        if(strcmp(tokens[0], "exit") == 0){ //exit program
            break;
        }
        else if(strcmp(tokens[0], "cd") == 0){
            if(tc > 1){
                if(strcmp(tokens[1], "~") == 0){
                    uid_t uid = getuid();
                    struct passwd *pw = getpwuid(uid);
                    if(chdir(pw->pw_dir) != 0){
                        fprintf(stderr, "Error: %s.\n", strerror(errno));
                    }
                }
                else if(chdir(tokens[1]) != 0){
                    fprintf(stderr, "Error: %s.\n", strerror(errno));
                }   
            }
            else{
                uid_t uid = getuid();
                struct passwd *pw = getpwuid(uid);
                if(chdir(pw->pw_dir) != 0){
                    fprintf(stderr, "Error: %s.\n", strerror(errno)) ;
                }
            }
        }
        else if(tokens[0] != NULL){
            struct sigaction action;
            memset(&action, 0, sizeof(struct sigaction));
            action.sa_handler = catch_signal;
            action.sa_flags = SA_RESTART;
            if(sigaction(SIGINT, &action, NULL) == -1){
                perror("sigaction");
                return EXIT_FAILURE;
            }
            pid_t pid;
            if((pid = fork()) == 0){
                if(tc > 1){
                    if(execvp(tokens[0], tokens) < 0){
                        fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    if(execlp(tokens[0], tokens[0], NULL) < 0){
                        fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }
                
            }
            if(signal_value == SIGINT){
                if(kill(pid, SIGINT) == -1){
                    fprintf(stderr, "Error: kill() failed. %s.\n", strerror(errno));
                    return EXIT_FAILURE;
                }
                signal_value = 0;
            }
            if(waitpid(pid, NULL, 0) < 0){
                fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
            }
        }
    }
    return EXIT_SUCCESS;
}