#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    long xy_size = 1000L * 1000L * 500L; // 4 GB when sizeof(long)=8
    long x_dim = 100;
    long y_dim = xy_size / x_dim;

    long **matrix = malloc(y_dim * sizeof(long *));
    if (!matrix) {
        fprintf(stderr, "Failed to allocate row pointer array (y_dim=%ld)\n", y_dim);
        return 1;
    }

    for (long y = 0; y < y_dim; y++) {
        matrix[y] = malloc(x_dim * sizeof(long));
        if (!matrix[y]) {
            fprintf(stderr, "Failed to allocate row %ld\n", y);
            // best-effort cleanup
            for (long i = 0; i < y; i++) free(matrix[i]);
            free(matrix);
            return 1;
        }
    }

    printf("Allocation complete (press Enter to continue...)\n");
    getchar();

    // Optional: initialize to zero for the variant described in Task A
    // for (long y = 0; y < y_dim; y++)
    // {
    //     memset(matrix[y], 0, x_dim * sizeof(long));
    // }
    // printf("Initialization complete (press Enter to continue...)\n");
    // getchar();

    for (long y = 0; y < y_dim; y++) {
        free(matrix[y]);
    }
    free(matrix);
    return 0;
}




