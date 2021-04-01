#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Author: Jacob Gerega
 * Pledge: I pledge my honor that I have abided by the Stevens Honor System.
 * Date: 3/30/21
 * */

int main(int argc, char *argv[])
{
    if(argc <= 1){
        printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pfind_to_sort[2], sort_to_parent[2];
    if (pipe(pfind_to_sort) < 0)
    {
        fprintf(stderr, "Error: Cannot create pipe pfind_to_sort. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (pipe(sort_to_parent) < 0)
    {
        fprintf(stderr, "Error: Cannot create pipe sort_to_parent. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    pid_t pid[2];
    if ((pid[0] = fork()) == 0)
    {
        close(pfind_to_sort[0]);
        dup2(pfind_to_sort[1], STDOUT_FILENO);
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);

        if (execl("pfind", "pfind", argv[1], argv[2], argv[3], argv[4], NULL) < 0)
        {
            fprintf(stderr, "Error: pfind failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if ((pid[1] = fork()) == 0)
    {
        close(pfind_to_sort[1]);
        dup2(pfind_to_sort[0], STDIN_FILENO);
        close(sort_to_parent[0]);
        dup2(sort_to_parent[1], STDOUT_FILENO);

        if (execlp("sort", "sort", NULL) < 0)
        {
            fprintf(stderr, "Error: sort failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    close(sort_to_parent[1]);
    dup2(sort_to_parent[0], STDIN_FILENO);
    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);

    char buffer[4096];
    int wc = 0;
    while (1)
    {
        ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("read()");
                exit(EXIT_FAILURE);
            }
        }
        else if (count == 0)
        {
            break;
        }
        else
        {
            for(int i = 0; i < count; i++){
                if(buffer[i] == '\n'){
                    wc++;
                }
            }
            write(STDOUT_FILENO, buffer, count);
            printf("Total matching: %d\n", wc);
        }
    }
    close(sort_to_parent[0]);
    wait(NULL);
    wait(NULL);
    return EXIT_SUCCESS;
}