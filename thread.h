#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include "vehicle.h"

struct ThreadData {
    pthread_t thread_id;
    struct Vehicle *vehicle;
};

void thread_start(struct ThreadData *thread);
void thread_join(struct ThreadData *thread);

#endif
