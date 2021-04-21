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


pthread_mutex_t lock;
bool *low_primes, *high_primes;
int ub;

typedef struct arg_struct {
    int start;
    int end;
    int si;
} thread_args;

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
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    int a = 29; //lower bound wanted by user
    int b = 90; //upper bound wanted by user
    int t = 12; //threads wanted by user
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
        t = b-a;
    }
    int cpt[t];
    int rem = (b-a) % t;
    for(int i = 0; i < t; i++){
        cpt[i] = (b-a) / t; //count / no of threads
        if(rem > 0){
            cpt[i]++;
            rem--;
        }
    }
    
    pthread_t threads[t]; //total threads
    thread_args targs[t]; //total thread arguments

    int start = a;
    int si = 0;
    for(int i = 0; i < t; i++){ //initialize all threads
        targs[i].start = start;
        targs[i].end = start+cpt[i];
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
    for(int i = 0; i < t; i++){ //join all threads
        if(pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "Warning: Thread %d did not join properly.\n",
                    i + 1);
        }
    }
    if ((retval = pthread_mutex_destroy(&lock)) != 0) { //destroy lock
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
        free(low_primes);
        free(high_primes);
        return EXIT_FAILURE;
    }

    printf("Finding all prime numbers between %d and %d.\n%d segments:\n", a, b, t);
    for(int i = 0; i < t; i++){
        printf("\t[%d, %d]\n", targs[i].start, targs[i].end);
    }
    free(low_primes);
    free(high_primes);
    return EXIT_SUCCESS;

}