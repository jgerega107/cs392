#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

void rdir(char* p){
    DIR *dir;
    if((dir = opendir(p)) == NULL){
        fprintf(stderr, "Cannot open dir");
        return;
    }

    struct dirent *entry;
    struct stat buf;
    char filename[PATH_MAX+1];
    size_t pathlen = 0;

    filename[0] = '\0';
    if(strcmp(p, "/")){
        strncpy(filename, p, PATH_MAX);
    }
    pathlen = strlen(filename) + 1;
    filename[pathlen - 1] = '/';
    filename[pathlen] = '\0';
    
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        strncpy(filename + pathlen, entry->d_name, PATH_MAX - pathlen);
        if(lstat(filename, &buf) < 0){
            fprintf(stderr, "Error: Cannot stat file");
            continue;
        }
        if(entry->d_type == DT_DIR){
            printf("%s [dir]\n", filename);
            rdir(filename);
        }
        else{
            printf("%s\n", filename);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]){
    char path[PATH_MAX];

    int dir = 0;
    int perm = 0;
    int opt;
    while((opt = getopt(argc, argv, "d:p:")) != -1){
        switch(opt){
            case 'd':
                dir = 1;
                if(realpath(optarg, path) == NULL){
                    fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", optarg, strerror(errno));
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                perm = 1;
                if(strlen(optarg) == 9){
                    for(int i = 0; i < 9; i+=3){
                        if(!((optarg[i] == 'r' || optarg[i] == '-') &&
                        (optarg[i+1] == 'w' || optarg[i+1] == '-') &&
                        (optarg[i+2] == 'x' || optarg[i+2] == '-'))){
                            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", optarg);
                            return EXIT_FAILURE;
                        }
                    }
                }
                else{
                    fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                printf("Error: Unknown option '-%c' received.\n", optopt);
                return EXIT_FAILURE;
        }
    }
    rdir(path);
    return EXIT_SUCCESS;
}