#include <stdio.h>
#include <stdlib.h>
#include "array.h"

static void print_cb(long v) { printf(" %ld", v); }

int main(void) {
    Array a = array_new(2);

    for (long i = 1; i <= 10; i++) {
        array_insertBack(&a, i);
        printf("Inserted %ld, length=%ld, capacity=%ld\n", i, array_length(a), a.capacity);
    }

    printf("Final array:");
    array_foreach(a, print_cb);
    printf("\n");

    array_destroy(a);
    return 0;
}


