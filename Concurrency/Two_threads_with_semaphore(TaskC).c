#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

long global_variabel = 0;

pthread_barrier_t barrier;
sem_t semaphore;

void* fn(void* args) {

    int local_variable = 0;

    pthread_barrier_wait(&barrier);

    for (int i = 0; i < 1000000; i++) {

        sem_wait(&semaphore);
        global_variabel++;
        sem_post(&semaphore);
        local_variable++;
    }

    printf("Local variable for thread %d: %d\n", args, local_variable);

    return NULL;
}

int main() {

    pthread_t thread1;
    pthread_t thread2;

    sem_init(&semaphore, 0, 1);

    pthread_barrier_init(&barrier, NULL, 2);

    pthread_create(&thread1, NULL, fn, (void*)1);
    pthread_create(&thread2, NULL, fn, (void*)2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    pthread_barrier_destroy(&barrier);

    sem_destroy(&semaphore);

    printf("Global variable: %ld", global_variabel);

    return 0; // Success
}