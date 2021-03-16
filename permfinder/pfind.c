#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

//Author: Jacob Gerega
//Pledge: I pledge my honor that I have abided by the Stevens Honor System.

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

char *pms;

void rdir(char *p)
{
    DIR *dir;
    if ((dir = opendir(p)) == NULL)
    {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", p, strerror(errno));
        return;
    }
    struct dirent *entry;
    struct stat buf;
    char filename[PATH_MAX + 1];
    size_t pathlen = 0;

    filename[0] = '\0';
    if (strcmp(p, "/"))
    {
        strncpy(filename, p, PATH_MAX);
    }
    pathlen = strlen(filename) + 1;
    filename[pathlen - 1] = '/';
    filename[pathlen] = '\0';

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        strncpy(filename + pathlen, entry->d_name, PATH_MAX - pathlen);
        if (lstat(filename, &buf) < 0)
        {
            fprintf(stderr, "Error: Cannot stat file\n");
            continue;
        }
        else
        {
            char *fpms = malloc(10);
            fpms[9] = '\0';
            for (int i = 0; i < 9; i += 3)
            {
                if (buf.st_mode & perms[i])
                {
                    fpms[i] = 'r';
                }
                else
                {
                    fpms[i] = '-';
                }
                if (buf.st_mode & perms[i + 1])
                {
                    fpms[i + 1] = 'w';
                }
                else
                {
                    fpms[i + 1] = '-';
                }
                if (buf.st_mode & perms[i + 2])
                {
                    fpms[i + 2] = 'x';
                }
                else
                {
                    fpms[i + 2] = '-';
                }
            }
            if (strcmp(pms, fpms) == 0)
            {
                printf("%s\n", filename);
            }
            free(fpms);
            if (entry->d_type == DT_DIR)
            {
                rdir(filename);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    char path[PATH_MAX];
    char fold[PATH_MAX];
    int dir = 0;
    int nodiropts = 0;
    int perm = 0;
    int nopermopts = 0;
    int opt;
    while ((opt = getopt(argc, argv, ":d:p:h")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
            return EXIT_SUCCESS;
        case 'd':
            dir = 1;
            strcpy(fold, optarg);
            break;
        case 'p':
            perm = 1;
            //check perms string
            if (strlen(optarg) == 9)
            {
                for (int i = 0; i < 9; i += 3)
                {
                    if (!((optarg[i] == 'r' || optarg[i] == '-') &&
                          (optarg[i + 1] == 'w' || optarg[i + 1] == '-') &&
                          (optarg[i + 2] == 'x' || optarg[i + 2] == '-')))
                    {
                        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", optarg);
                        return EXIT_FAILURE;
                    }
                }
            }
            else
            {
                fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", optarg);
                return EXIT_FAILURE;
            }
            pms = malloc(10);
            strcpy(pms, optarg);
            break;
        case ':':
            if (optopt == 'd')
            {
                nodiropts = 1;
            }
            else if (optopt == 'p')
            {
                nopermopts = 1;
            }
            break;
        case '?':
            printf("Error: Unknown option '-%c' received.\n", optopt);
            return EXIT_FAILURE;
        }
    }

    if (dir == 0 && perm == 0 && nodiropts == 0 && nopermopts == 0)
    {
        printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
        return EXIT_SUCCESS;
    }
    if (dir == 0 || nodiropts == 1)
    {
        printf("Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    }
    if (perm == 0 || nopermopts == 1)
    {
        printf("Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    }

    if (realpath(fold, path) == NULL)
    {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", fold, strerror(errno));
        return EXIT_FAILURE;
    }
    rdir(path);
    free(pms);
    return EXIT_SUCCESS;
}