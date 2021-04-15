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
#define DEFAULT "\x1b[0m"

/**
 * Author: Jacob Gerega
 * Pledge: I pledge my honor that I have abided by the Stevens Honor System.
 ***/

volatile sig_atomic_t signal_value = 0;

void catch_signal(int sig)
{
    signal_value = sig;
}

int main(int argc, char *argv[])
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    while (true)
    {
        char cwd[PATH_MAX]; //current working directory
        if (getcwd(cwd, PATH_MAX) == NULL)
        {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        printf("[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT); //print curr directory
        char line[PATH_MAX];                            //line input
        fflush(stdout);
        int br = read(STDIN_FILENO, line, PATH_MAX);

        if (br < 0)
        {
            //read failed, was it an interrupt?
            if (errno == EINTR)
            {
                printf("\n");
                continue;
            }
            else
            {
                return EXIT_FAILURE;
            }
        }
        //read in line input
        line[br - 1] = '\0';
        char *token = strtok(line, " \t");
        char *tokens[PATH_MAX];
        memset(tokens, 0, sizeof(tokens));
        int tc = 0;
        while (token != NULL)
        {
            tokens[tc] = token;
            tc++;
            token = strtok(NULL, " \t");
        }
        if (tokens[0] == NULL)
        {
            continue;
        }
        else if (strcmp(tokens[0], "exit") == 0)
        { //exit program
            break;
        }
        else if (strcmp(tokens[0], "cd") == 0)
        {
            if (tc > 2)
            {
                printf("Error: Too many arguments to cd.\n");
            }
            else if (tc > 1)
            {
                if (strcmp(tokens[1], "~") == 0)
                {
                    uid_t uid = getuid();
                    struct passwd *pw = getpwuid(uid);
                    if (pw == NULL)
                    {
                        fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
                        continue;
                    }
                    if (chdir(pw->pw_dir) != 0)
                    {
                        fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", pw->pw_dir, strerror(errno));
                        continue;
                    }
                }
                else if (chdir(tokens[1]) != 0)
                {
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", tokens[1], strerror(errno));
                    continue;
                }
            }
            else
            {
                uid_t uid = getuid();
                struct passwd *pw = getpwuid(uid);
                if (chdir(pw->pw_dir) != 0)
                {
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", pw->pw_dir, strerror(errno));
                    continue;
                }
            }
        }
        else if (tokens[0] != NULL)
        {
            pid_t pid;
            if ((pid = fork()) == 0)
            {
                if (execvp(tokens[0], tokens) < 0)
                {
                    fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            if (waitpid(pid, NULL, 0) < 0)
            {
                if (signal_value == SIGINT)
                {
                    printf("\n");
                    continue;
                }
                fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
            }
        }
    }
    return EXIT_SUCCESS;
}