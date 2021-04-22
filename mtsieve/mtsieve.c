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
#include <math.h>
#include <pthread.h>
#include <sys/sysinfo.h>

/**
 * Author: Jacob Gerega
 * Pledge: I pledge my honor that I have abided by the Stevens Honor System.
 **/


pthread_mutex_t lock;
bool *low_primes, *high_primes;
int count = 0;
int ub;

typedef struct arg_struct {
    int start;
    int end;
    int si;
} thread_args;

void display_help(char* cmd){
    printf("Usage: %s -s <starting value> -e <ending value> -t <num threads>\n", cmd);
}

void *part_sieve(void *ptr){
    thread_args* ta = (thread_args*)ptr;
    for(int p = 2; p <= ub; p++){
        if(low_primes[p]){
            int i = ceil((double)ta->start/p) * p - ta->start;
            if(ta->start <= p){
                i = i + p;
            }
            for(int k = i; k <= ta->end-ta->start; k+=p){
                high_primes[k+ta->si] = false;
            }
        }
    }
    for(int i = 0; i < ta->end-ta->start; i++){
        int temp_count = 0;
        if(high_primes[ta->si+i]){
            int curr = ta->start+i;
            while(curr != 0){
                if(curr % 10 == 3){
                    temp_count++;
                }
                curr /= 10;
            }
            if(temp_count >= 2){
                if(pthread_mutex_lock(&lock) != 0){
                    fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(errno));
                }
                count++;
                if(pthread_mutex_unlock(&lock) != 0){
                    fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(errno));
                }
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    long a = -1; //lower bound wanted by user
    long b = -1; //upper bound wanted by user
    long t = -1; //threads wanted by user 
    int opt = 0;
    while ((opt = getopt(argc, argv, ":s:e:t:")) != -1) {
        switch (opt) {
            case 's':
                a = strtol(optarg, NULL, 10);
                if(a > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'\n", opt);
                    return EXIT_FAILURE;
                }
                if(a == 0){
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, opt);
                    return EXIT_FAILURE;
                }
                break;
            case 'e':
                b = strtol(optarg, NULL, 10);
                if(b > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'\n", opt);
                    return EXIT_FAILURE;
                }
                if(b == 0){
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, opt);
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                t = strtol(optarg, NULL, 10);
                if(t > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'\n", opt);
                    return EXIT_FAILURE;
                }
                if(t == 0){
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, opt);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                fprintf(stderr, "Error: Non-option argument '-%c' supplied.\n", optopt);
                return EXIT_FAILURE;
            case ':':
                fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
                return EXIT_FAILURE;
        }
    }
    if(optind < argc){
        fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind++]);
        return EXIT_FAILURE;
    }
    if(a==-1 && b == -1 && t == -1){
        display_help(argv[0]);
        return EXIT_FAILURE;
    }
    if(a == -1){
        fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
        return EXIT_FAILURE;
    }
    else if(a < 2){
        fprintf(stderr, "Error: Starting value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    if(b == -1){
        fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
        return EXIT_FAILURE;
    }
    else if(b < 2){
        fprintf(stderr, "Error: Ending value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    else if(b < a){
        fprintf(stderr, "Error: Ending value must be >= starting value.\n");
        return EXIT_FAILURE;
    }
    if(t == -1){
        fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
        return EXIT_FAILURE;
    }
    else if(t < 1){
        fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
        return EXIT_FAILURE;
    }
    else if(t > get_nprocs()){
        fprintf(stderr, "Error: Number of threads exceed twice the number of processors(%d)\n", get_nprocs());
        return EXIT_FAILURE;
    }
    ub = (int) sqrt(b); //upper bound for low_primes
    if((low_primes = (bool *)malloc((ub+1) * sizeof(bool))) == NULL){ //allocate low_primes
        fprintf(stderr,
            "Error: Cannot allocate memory for array of size '%d'. %s.\n",
            ub+1, strerror(errno));
        return EXIT_FAILURE;
    }
    for(int i = 0; i <= ub; i++){ //fill with true
        low_primes[i] = true;
    }
    for(int p = 2; p*p <= ub; p++){ // calculate low_primes
        if(low_primes[p]){
            for(int i = p*p; i<= ub; i+=p){
                low_primes[i] = false;
            }
        }
    }
    if((high_primes = (bool *)malloc((b-a+1) * sizeof(bool))) == NULL){ //allocate high_primes
        fprintf(stderr,
            "Error: Cannot allocate memory for array of size '%d'. %s.\n",
            ub+1, strerror(errno));
        return EXIT_FAILURE;
    }
    for(int i = 0; i <= b-a; i++){ //fill with true
        high_primes[i] = true;
    }

    int retval; //mutex return value
    if((retval = pthread_mutex_init(&lock, NULL)) != 0){ //initialize mutex
        fprintf(stderr, "Error: Cannot create mutex. %s. \n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(t > b-a){ //if threads are greater than count
        if(b==a){
            t=1;
        }
        else{
            t = b-a;
        }
    }
    int cpt[t];
    int rem = (b-a+1) % t;
    for(int i = 0; i < t; i++){
        cpt[i] = (b-a+1) / t; //count / no of threads
    }
    int i = 0;
    while(rem > 0){
        if(i >= t){
            i = 0;
        }
        else{
            cpt[i]++;
            rem--;
            i++;
        }
    }
    
    pthread_t threads[t]; //total threads
    thread_args targs[t]; //total thread arguments

    int start = a;
    int si = 0;
    for(int i = 0; i < t; i++){ //initialize all threads
        targs[i].start = start;
        targs[i].end = start+cpt[i]-1;
        targs[i].si = si;
        si+=cpt[i];
        start+=cpt[i];
        if(pthread_create(&threads[i], NULL, part_sieve, &targs[i]) != 0){
            fprintf(stderr, "Error: cannot create thread %d. %s.\n", i+1, strerror(errno));
            free(low_primes);
            free(high_primes);
            return EXIT_FAILURE;
        }
    }
    printf("Finding all prime numbers between %ld and %ld.\n", a, b);
    if(t == 1){
        printf("%ld segment\n   [%d, %d]\n", t, targs[0].start, targs[0].end);
    }
    else{
        printf("%ld segments\n", t);
        for(int i = 0; i < t; i++){
            printf("   [%d, %d]\n", targs[i].start, targs[i].end);
        }
    }
    
    for(int i = 0; i < t; i++){ //join all threads
        if(pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "Warning: Thread %d did not join properly.\n",
                    i + 1);
        }
    }
    if ((retval = pthread_mutex_destroy(&lock)) != 0) { //destroy lock
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
    }

    printf("Total primes between %ld and %ld with two or more '3' digits: %d\n", a, b, count);
    free(low_primes);
    free(high_primes);
    return EXIT_SUCCESS;
}