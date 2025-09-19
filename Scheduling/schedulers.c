#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}


// FCFS non-preemptive scheduler
void first_come_first_served(struct Task **tasks, int taskCount, int timeout) {

    int tasksFinished = 0;
    int shortestArrivalTime;
    int priorityId;
    struct Task *runningTask = NULL;
    
    while (tasksFinished < taskCount && globalTime < timeout) {

        if (runningTask == NULL) {

            shortestArrivalTime = 100000000;
            priorityId = -1;

            for (int i = 0; i < taskCount; i++) {

                if (tasks[i]->arrivalTime < shortestArrivalTime
                    && tasks[i]->state != finished
                    && tasks[i]->arrivalTime <= globalTime) {

                    shortestArrivalTime = tasks[i]->arrivalTime;
                    priorityId = i;
                }
            }

            if (priorityId == -1) {

                pthread_mutex_lock(&timeMutex);
                pthread_cond_wait(&timeCond, &timeMutex);
                pthread_mutex_unlock(&timeMutex);
                continue;

            } else {

                runningTask = tasks[priorityId];
                set_task_state(runningTask, running);
                runningTask->startTime = globalTime;
            }
        }

        while (runningTask->state != finished && globalTime < timeout) {

            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }

        if (runningTask->state == finished) {

            runningTask = NULL;
            tasksFinished++;
        }
    }
}


// Shortest process non-preemptive scheduler
void shortest_process_next(struct Task **tasks, int taskCount, int timeout) {

    int tasksFinished = 0;
    int shortestProcessTime;
    int priorityId;
    struct Task *runningTask = NULL;
    
    while (tasksFinished < taskCount && globalTime < timeout) {

        if (runningTask == NULL) {

            shortestProcessTime = 100000000;
            priorityId = -1;

            for (int i = 0; i < taskCount; i++) {

                if (tasks[i]->totalRuntime < shortestProcessTime
                    && tasks[i]->state != finished
                    && tasks[i]->arrivalTime <= globalTime) {

                    shortestProcessTime = tasks[i]->totalRuntime;
                    priorityId = i;
                }
            }

            if (priorityId == -1) {

                pthread_mutex_lock(&timeMutex);
                pthread_cond_wait(&timeCond, &timeMutex);
                pthread_mutex_unlock(&timeMutex);
                continue;

            } else {

                runningTask = tasks[priorityId];
                set_task_state(runningTask, running);
                runningTask->startTime = globalTime;
            }
        }

        while (runningTask->state != finished && globalTime < timeout) {

            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }

        if (runningTask->state == finished) {

            runningTask = NULL;
            tasksFinished++;
        }
    }
}


// Formula for highest response ratio
double response_ratio (struct Task *task) {

    return 1 + (double)(globalTime - task->arrivalTime) / (double)task->totalRuntime;
}

// Highest response ratio, non-preemptive scheduler
void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout) {
    
    int tasksFinished = 0;
    double responseRatio;
    int priorityId;
    struct Task *runningTask = NULL;
    
    while (tasksFinished < taskCount && globalTime < timeout) {

        if (runningTask == NULL) {

            responseRatio = -1.0;
            priorityId = -1;

            for (int i = 0; i < taskCount; i++) {

                if (tasks[i]->state != finished
                    && tasks[i]->arrivalTime <= globalTime
                    && response_ratio(tasks[i]) > responseRatio) {

                    responseRatio = response_ratio(tasks[i]);
                    priorityId = i;
                }
            }

            if (priorityId == -1) {

                pthread_mutex_lock(&timeMutex);
                pthread_cond_wait(&timeCond, &timeMutex);
                pthread_mutex_unlock(&timeMutex);
                continue;

            } else {

                runningTask = tasks[priorityId];
                set_task_state(runningTask, running);
                runningTask->startTime = globalTime;
            }
        }

        while (runningTask->state != finished && globalTime < timeout) {

            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }

        if (runningTask->state == finished) {

            runningTask = NULL;
            tasksFinished++;
        }
    }
}


// Shortest remaining time, preemptive scheduler
void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum) {

    int tasksFinished = 0;
    int shortestRemaining;
    int priorityId;
    struct Task *runningTask = NULL;
    
    while (tasksFinished < taskCount && globalTime < timeout) {

        if (runningTask == NULL) {

            shortestRemaining = 1000000;
            priorityId = -1;

            for (int i = 0; i < taskCount; i++) {

                if (tasks[i]->state != finished
                    && tasks[i]->arrivalTime <= globalTime) {

                    int taskTimeRemaining = tasks[i]->totalRuntime - tasks[i]->currentRuntime;
                    
                    if (taskTimeRemaining < shortestRemaining) {

                        shortestRemaining = taskTimeRemaining;
                        priorityId = i;
                    }
                }
            }

            if (priorityId == -1) {

                pthread_mutex_lock(&timeMutex);
                pthread_cond_wait(&timeCond, &timeMutex);
                pthread_mutex_unlock(&timeMutex);
                continue;

            } else {

                runningTask = tasks[priorityId];
                set_task_state(runningTask, running);
                runningTask->startTime = globalTime;
            }
        }

        wait_for_rescheduling(quantum, runningTask);

        if (runningTask->state == finished) {

            tasksFinished++;

        } else {

            set_task_state(runningTask, preempted);
        }
        runningTask = NULL;
    }
}


// Multiple qeueus based on if the threads have finished on time, preemptive scheduler
void feedback(struct Task **tasks, int taskCount, int timeout, int quantum) {

    // Lets implement 3 levels
    // At the first level, threads will run for a maximum of 1 quantum
    // At the second level, they will run for a maximum of 2
    // At the 3rd they will run indefinetly
    int qeueu[taskCount];

    // Initialize all threads to be in Q0
    for (int i = 0; i < taskCount; i++) qeueu[i] = 0;

    int tasksFinished = 0;
    int priorityId;
    struct Task *runningTask = NULL;
    
    while (tasksFinished < taskCount && globalTime < timeout) {

        int priorityQeueu = 2;
        int shortestArrivalTime = 10000000;
        int priorityId = -1;

        for (int i = 0; i < taskCount; i++) {

            if (tasks[i]->state != finished
                && tasks[i]->arrivalTime <= globalTime) {

                if (qeueu[i] < priorityQeueu
                    || qeueu[i] == priorityQeueu
                    && tasks[i]->arrivalTime < shortestArrivalTime) {

                    priorityId = i;
                    priorityQeueu = qeueu[i];
                    shortestArrivalTime = tasks[i]->arrivalTime;
                }
            }
        }

        if (priorityId == -1) {

            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;

        } else {

            runningTask = tasks[priorityId];
            runningTask->startTime = globalTime;
            set_task_state(runningTask, running);
        }
        switch (priorityQeueu) {

            case 0:
                wait_for_rescheduling(quantum, runningTask);
                break;
            case 1:
                wait_for_rescheduling(2 * quantum, runningTask);
                break;
            case 2:
                wait_for_rescheduling(1000000, runningTask);
                break;
        }

        if (runningTask->state == finished) {

            tasksFinished++;

        } else {

            set_task_state(runningTask, preempted);
            if (qeueu[priorityId] < 2) qeueu[priorityId]++;
        }
        runningTask = NULL;
    }
}
