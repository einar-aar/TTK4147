#include <pthread.h>
#include <stdio.h>

long global_variabel = 0;

pthread_barrier_t barrier;

void* fn(void* args) {

    int local_variable = 0;

    pthread_barrier_wait(&barrier);

    for (int i = 0; i < 1000000; i++) {

        global_variabel++;
        local_variable++;
    }

    printf("Global variable: %d\nLocal variable: %d\n", global_variabel, local_variable);

    return NULL;
}

int main() {

    pthread_t thread1;
    pthread_t thread2;

    pthread_barrier_init(&barrier, NULL, 2);

    pthread_create(&thread1, NULL, fn, NULL);
    pthread_create(&thread2, NULL, fn, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    pthread_barrier_destroy(&barrier);

    return 0; // Success
}