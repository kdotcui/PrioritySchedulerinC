#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "vehicle.h"
#include "thread.h"

void thread_start(struct ThreadData *thread) {
    int result = pthread_create(&(thread->thread_id), NULL, run, thread->vehicle);
    if (result != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }
}

void thread_join(struct ThreadData *thread) {
    int result = pthread_join(thread->thread_id, NULL);
    if (result != 0) {
        perror("pthread_join failed");
        exit(EXIT_FAILURE);
    }
}
