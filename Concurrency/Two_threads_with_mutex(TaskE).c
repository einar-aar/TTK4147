#include <pthread.h>
#include <stdio.h>

long global_variabel = 0;

pthread_barrier_t barrier;
pthread_mutex_t mux;

void* fn(void* args) {

    int local_variable = 0;

    pthread_barrier_wait(&barrier);

    for (int i = 0; i < 1000000; i++) {

        pthread_mutex_lock(&mux);
        global_variabel++;
        pthread_mutex_unlock(&mux);
        local_variable++;
    }

    printf("Local variable for thread %d: %d\n", (int*)args, local_variable);

    return NULL;
}

int main() {

    pthread_t thread1;
    pthread_t thread2;

    // Define and init mutex
    pthread_mutexattr_t mux_attr;
    pthread_mutexattr_init(&mux_attr);
    pthread_mutexattr_setprotocol(&mux_attr, PTHREAD_PRIO_INHERIT);
    pthread_mutex_init(&mux, &mux_attr);

    pthread_barrier_init(&barrier, NULL, 2);

    pthread_create(&thread1, NULL, fn, (void*)1);
    pthread_create(&thread2, NULL, fn, (void*)2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    pthread_barrier_destroy(&barrier);

    pthread_mutex_destroy(&mux);
    pthread_mutexattr_destroy(&mux_attr);

    printf("Global variable: %ld\n", global_variabel);

    return 0; // Success
}