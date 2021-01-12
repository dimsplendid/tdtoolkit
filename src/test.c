#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char * Str;

typedef struct _test {
    Str str;
    void * value;
} * test;

int new_test(test * self) {
    * self = calloc(1, sizeof(test));
    char * str = calloc(100, sizeof(char));
    (*self)->str = str;

    return 0;
}

int main(void) {
    char * h = "hello world";
    test new_data = NULL;
    new_test(&new_data);
    printf("copy str\n");
    strncpy(new_data->str, h,100);
    printf("%s\n", new_data->str);

    return 0;
}