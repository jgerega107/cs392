
#include <stdio.h>
#include <string.h>

void swap(void *a, void *b, size_t size) {
    char* xa = (char *)a;
    char* xb = (char *)b;
    char temp = *xa;
    for(int i = 0; i < size; i++){
        temp = *xa;
        *xa++ = *xb;
        *xb++ = temp;
    }
}

int main(int argc, char **argv) {
    char *a = "test";
    char *b = "this";
    size_t c = (sizeof(char)*4+1);
    swap((void *) &a, (void *) &b, c);
    printf("a: %s, b: %s\n", a, b);
    return 0;
}
