#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Author: Jacob Gerega
 * Pledge: I pledge my honor that I have abided by the Stevens Honor System.
 **/


volatile sig_atomic_t signal_val = 0;

void catch_signal(int sig) {
    signal_val = sig;
}

/**
 * Description:
 * The 'snooze' program takes in a single parameter, which is the number
 * of seconds (no less than 1) the program should sleep.
 *
 * It catches the SIGINT signal. When doing so, it should stop sleeping and
 * print how long it actually slept.
 *
 * For example, if the user runs "./snooze 5" and then types CTRL+C after 3
 * seconds, the program should output:
 * Slept for 3 of the 5 seconds allowed.
 *
 * If the user runs "./snooze 5" and never types CTRL+C, the program should
 * output:
 * Slept for 5 of the 5 seconds allowed.
 */
int main(int argc, char *argv[]) {
    // TODO: Print the usage message "Usage: %s <seconds>\n" and return in
    // failure if the argument <seconds> is not present.

    // TODO: Parse the argument, and accept only a positive int. If the argument
    // is invalid, error out with the message:
    // "Error: Invalid number of seconds '%s' for max snooze time.\n",
    // where %s is whatever argument the user supplied.
    if(argc != 2){
        printf("Usage: %s <seconds>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int sleepsecs = atoi(argv[1]);
    if(sleepsecs <= 0){
        printf("Error: Invalid number of seconds '%s' for max snooze time.\n", argv[1]);
        return EXIT_FAILURE;
    }

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = catch_signal;
    action.sa_flags = SA_RESTART;

    if(sigaction(SIGINT, &action, NULL) == -1){
        return EXIT_FAILURE;
    }

    int i = 0;
    while(i < sleepsecs && signal_val != SIGINT){
        sleep(1);
        i++;
    }

    // TODO: Create a sigaction to handle SIGINT.

    // TODO: Loop and sleep for 1 second at a time, being able to stop looping
    // when the signal is processed.

    printf("Slept for %d of the %d seconds allowed.\n", i, sleepsecs);
    return EXIT_SUCCESS;
}
