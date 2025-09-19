#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int main(void) {
    // Small capacity to demonstrate overflow when no growth is implemented
    Array a = array_new(3);

    // Insert within capacity
    array_insertBack(&a, 10);
    array_insertBack(&a, 20);
    array_insertBack(&a, 30);

    // Exceed capacity intentionally (undefined behavior without growth)
    // This should trigger AddressSanitizer if compiled with -fsanitize=address
    array_insertBack(&a, 40);

    array_print(a);

    array_destroy(a);
    return 0;
}



