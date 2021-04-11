#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>


#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

int main(int argc, char* argv[]){
    while(true){
        size_t buflen = 64;
        char cwd[buflen];
        printf("\n[%s%s%s]$ ", BRIGHTBLUE, getcwd(cwd, buflen), DEFAULT);
        char* line[buflen];
        getline(line, &buflen, stdin);

        if(strcmp(*line, "exit\n") == 0){
            break;
        }
        else if(strstr(*line, "cd")){
            char* dir = strchr(*line, ' ');
            //goto directory given
            if(dir){
                dir++;
                dir[strlen(dir)-1] = '\0';
                if(chdir(dir) != 0){
                    fprintf(stderr, "Error: %s.\n", strerror(errno));
                }   
            }
            //else go home
            else if(!dir || strcmp(dir, "~") == 0){
                uid_t uid = getuid();
                struct passwd *pw = getpwuid(uid);
                if(chdir(pw->pw_dir) != 0){
                    fprintf(stderr, "Error: %s.\n", strerror(errno));
                }
            }
            printf("%ld\n", strlen(dir));

        }
        else{

        }
    }
    return EXIT_SUCCESS;
}