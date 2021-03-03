/*******************************************************************************
 * Name        : sort.c
 * Author      : Jacob Gerega
 * Date        : 3/2/21
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */

/**
 * Reads data from filename into an already allocated 2D array of chars.
 * Exits the entire program if the file cannot be opened.
 */
size_t read_data(char *filename, char **data) {
    // Open the file.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
        free(data);
        exit(EXIT_FAILURE);
    }

    // Read in the data.
    size_t index = 0;
    char str[MAX_STRLEN + 2];
    char *eoln;
    while (fgets(str, MAX_STRLEN + 2, fp) != NULL) {
        eoln = strchr(str, '\n');
        if (eoln == NULL) {
            str[MAX_STRLEN] = '\0';
        } else {
            *eoln = '\0';
        }
        // Ignore blank lines.
        if (strlen(str) != 0) {
            data[index] = (char *)malloc((MAX_STRLEN + 1) * sizeof(char));
            strcpy(data[index++], str);
        }
    }

    // Close the file before returning from the function.
    fclose(fp);

    return index;
}

void display_help(char *name){
    printf("Usage: %s [-i|-d] filename\n   -i: Specifies the file contains ints.\n   -d: Specifies the file contains doubles\n   filename: The file to sort.\n   No flags defaults to sorting strings.\n", name);
}

void display_arrayint(int *array, const int length) {

    for (int i = 0; i < length; i++) {
        printf("%d\n", *(array+(i)));
    }
}

void display_arraydb(double *array, const int length) {
    for (int i = 0; i < length; i++) {
        printf("%f\n", *(array+(i)));
    }
}

void display_arraystr(char **array, const int length) {
    for (int i = 0; i < length; i++) {
        printf("%s\n", *(array+(i)));
    }
}

int main(int argc, char **argv) {
    int flagc = 0;
    int it = 0;
    int db = 0;
    int opt;
    while((opt = getopt(argc, argv, ":id")) != -1){
        switch(opt){
            case 'i':
                flagc++;
                it = 1;
                break;
            case 'd':
                flagc++;
                db = 1;
                break;
            case '?':
                printf("Error: Unknown option '-%c' received.\n", optopt);
                display_help(argv[0]);
                return EXIT_FAILURE;
        }
    }
    //no flags and no files
    if(flagc == 0 && optind == argc){
        display_help(argv[0]);
        return EXIT_FAILURE;
    }
    //no files
    if(optind == argc){
        printf("Error: No input file specified.");
        return EXIT_FAILURE;
    }
    //too many files
    if(optind != argc){
        int x = optind;
        int count = 0;
        while(x < argc){
            x++;
            count++;
        }
        if(count > 1){
            printf("Error: Too many files specified.");
            return EXIT_FAILURE;
        }
        
    }
    //too many flags
    if(flagc > 1){
        printf("Error: Too many flags specified.");
        return EXIT_FAILURE;
    }
    char** data = (char **)malloc(MAX_ELEMENTS*MAX_STRLEN+2);
    size_t len = read_data(argv[optind], data);

    if(it == 1){
        int* intarr = (int *)malloc(len*sizeof(int));
        for(int i = 0; i < len; i++){
            intarr[i] = atoi(data[i]);
        }
        quicksort(intarr, len, sizeof(int), int_cmp);
        display_arrayint(intarr, len);
        free(intarr);
    }
    else if(db == 1){
        double* dbarr = (double *)malloc(len*sizeof(double));
        for(int i = 0; i < len; i++){
            dbarr[i] = atof(data[i]);
        }
        quicksort(dbarr, len, sizeof(double), dbl_cmp);
        display_arraydb(dbarr, len);
        free(dbarr);
    }
    else{
        quicksort(data, len, sizeof(char*), str_cmp);
        display_arraystr(data, len);
    }
    for(int i = 0; i < len; i++){
            free(data[i]);
        }
    free(data);
    
    return EXIT_SUCCESS;
}
