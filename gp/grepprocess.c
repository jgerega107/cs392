#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <process name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int ps_to_grep[2], grep_to_parent[2];
    if (pipe(ps_to_grep) < 0) {
        fprintf(stderr, "Error: Cannot create pipe ps_to_grep. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }
    if (pipe(grep_to_parent) < 0) {
        fprintf(stderr, "Error: Cannot create pipe grep_to_parent. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    pid_t pid[2];
    if ((pid[0] = fork()) == 0) {
        // ps
        close(ps_to_grep[0]);
        dup2(ps_to_grep[1], STDOUT_FILENO);

        // Close all unrelated file descriptors.
        close(grep_to_parent[0]);
        close(grep_to_parent[1]);

        if (execlp("ps", "ps", "-A", NULL) < 0) {
            fprintf(stderr, "Error: ps failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if ((pid[1] = fork()) == 0) {
        // grep
        close(ps_to_grep[1]);
        dup2(ps_to_grep[0], STDIN_FILENO);
        close(grep_to_parent[0]);
        dup2(grep_to_parent[1], STDOUT_FILENO);

        if (execlp("grep", "grep", "-i", argv[1], NULL) < 0) {
            fprintf(stderr, "Error: grep failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    close(grep_to_parent[1]);
    // This step is not required. You could read directly from grep_to_parent[0] 
    // instead of from STDIN_FILENO.
    dup2(grep_to_parent[0], STDIN_FILENO);

    // Close all unrelated file descriptors.
    close(ps_to_grep[0]);
    close(ps_to_grep[1]);

    int counter = 0;
    char buffer[4096];
    while (1) {
        ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("read()");
                exit(EXIT_FAILURE);
            }
        } else if (count == 0) {
            break;
        } else {
            write(STDOUT_FILENO, buffer, count);
        }
    }
    printf("Count: %d\n", counter);
    close(grep_to_parent[0]);
    // Don't forget to wait for both children.
    wait(NULL);
    wait(NULL);

    return EXIT_SUCCESS;
}
