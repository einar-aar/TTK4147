#define _GNU_SOURCE
#include <pthread.h>
#include <wiringPi.h>


#define PININ_A   8
#define PINOUT_A  9
#define PININ_B   7
#define PINOUT_B  15
#define PININ_C   16
#define PINOUT_C  1

// Initialize input/output
static void io_init(void) {

    wiringPiSetup();

    pinMode(PININ_A,  INPUT);
    pinMode(PININ_B,  INPUT);
    pinMode(PININ_C,  INPUT);

    pinMode(PINOUT_A, OUTPUT);
    pinMode(PINOUT_B, OUTPUT);
    pinMode(PINOUT_C, OUTPUT);

    digitalWrite(PINOUT_A, HIGH);
    digitalWrite(PINOUT_B, HIGH);
    digitalWrite(PINOUT_C, HIGH);
}

// Choose specific CPU
int set_cpu(int cpu_id)
{
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Thread functions
static void *thread_A(void *arg) {

    set_cpu(1);

    while (1) {

        while (digitalRead(PININ_A) == HIGH);
        digitalWrite(PINOUT_A, LOW);
        while (digitalRead(PININ_A) == LOW);
        digitalWrite(PINOUT_A, HIGH);
    }

    return NULL;
}

static void *thread_B(void *arg) {

    set_cpu(1);

    while (1) {

        while (digitalRead(PININ_B) == HIGH);
        digitalWrite(PINOUT_B, LOW);
        while (digitalRead(PININ_B) == LOW);
        digitalWrite(PINOUT_B, HIGH);
    }

    return NULL;
}

static void *thread_C(void *arg) {

    set_cpu(1);

    while (1) {

        while (digitalRead(PININ_C) == HIGH);
        digitalWrite(PINOUT_C, LOW);
        while (digitalRead(PININ_C) == LOW);
        digitalWrite(PINOUT_C, HIGH);
    }

    return NULL;
}

int main(void) {

    io_init();

    pthread_t thread_a, thread_b, thread_c;

    pthread_create(&thread_a, NULL, thread_A, NULL);
    pthread_create(&thread_b, NULL, thread_B, NULL);
    pthread_create(&thread_c, NULL, thread_C, NULL);

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    pthread_join(thread_c, NULL);

    return 0;
}
