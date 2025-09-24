#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

// Eating algorithm

// Think for 1 sec
// Pick up left fork
// If holding left fork, pick up right fork
// If right fork unavailable, put down left fork
// Repeat until holding both forks
// Eat for 2 sec
// Put down forks

pthread_mutex_t fork1, fork2, fork3, fork4, fork5;
pthread_mutex_t* forks[] = {&fork1, &fork2, &fork3, &fork4, &fork5};

pthread_t philosopher1, philosopher2, philosopher3, philosopher4, philosopher5; 

void* typical_philosopher_behaviour(void* argc) {

    int philosopher_id = (int)(intptr_t)argc;

    int fork_left = philosopher_id - 1;
    int fork_right = (philosopher_id == 1) ? 4 : philosopher_id - 2;

    bool hungry = true;
    int holding_left = 0;
    int holding_right = 0;

    while (hungry) {

        // Think for 1 second
        sleep(1);

        // Try to take left fork, if failed go to think state
        holding_left = (pthread_mutex_lock(forks[fork_left]) == 0) ? 1 : 0;
        if (!holding_left) continue;
        printf("Philosopher %d picked up fork %d in his left hand\n", philosopher_id, fork_left + 1);

        struct timespec fork_deadline;
        clock_gettime(CLOCK_MONOTONIC, &fork_deadline);
        fork_deadline.tv_sec += 1;

        holding_right = (pthread_mutex_timedlock(forks[fork_right], &fork_deadline) == 0) ? 1 : 0;

        if (holding_right) {

            printf("Philosopher %d picked up fork %d in his right hand\n", philosopher_id, fork_right + 1);
            // Eat for 2 sec
            sleep(2);

            pthread_mutex_unlock(forks[fork_left]);
            holding_left = 0;

            pthread_mutex_unlock(forks[fork_right]);
            holding_right = 0;

            printf("Philosopher %d monologe: That was some mediocre food, i'm no longer hungry\n", philosopher_id);

            hungry = false;

        } else {

            printf("Philosopher %d monologe: My left fork is too heavy, i'm putting it down\n", philosopher_id);
            pthread_mutex_unlock(forks[fork_left]);
        }
    }

    return NULL;
}

int main() {

    // Set same clock in mutex as clock used for fork_deadline
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setclock(&attr, CLOCK_MONOTONIC);

    // Init mutexes
    pthread_mutex_init(&fork1, &attr);
    pthread_mutex_init(&fork2, &attr);
    pthread_mutex_init(&fork3, &attr);
    pthread_mutex_init(&fork4, &attr);
    pthread_mutex_init(&fork5, &attr);

    // Init threads
    pthread_create(&philosopher1, NULL, typical_philosopher_behaviour, (void *)(intptr_t)1);
    pthread_create(&philosopher2, NULL, typical_philosopher_behaviour, (void *)(intptr_t)2);
    pthread_create(&philosopher3, NULL, typical_philosopher_behaviour, (void *)(intptr_t)3);
    pthread_create(&philosopher4, NULL, typical_philosopher_behaviour, (void *)(intptr_t)4);
    pthread_create(&philosopher5, NULL, typical_philosopher_behaviour, (void *)(intptr_t)5);

    // Join threads
    pthread_join(philosopher1, NULL);
    pthread_join(philosopher2, NULL);
    pthread_join(philosopher3, NULL);
    pthread_join(philosopher4, NULL);
    pthread_join(philosopher5, NULL);

    printf("All philosophers are well fed\n");

    // Destroy mutexes
    pthread_mutex_destroy(&fork1);
    pthread_mutex_destroy(&fork2);
    pthread_mutex_destroy(&fork3);
    pthread_mutex_destroy(&fork4);
    pthread_mutex_destroy(&fork5);

    pthread_mutexattr_destroy(&attr);

    return 0;
}