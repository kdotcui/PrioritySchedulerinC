#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"
#include "vehicle.h"
#include "hashmap.h"
#include "priority_scheduler.h"

struct PriorityScheduler {
    pthread_mutex_t lock;
    pthread_cond_t lock_cv;
    HashMap *tunnel_map;
    int priority_counts[HIGHEST_PRIORITY + 1];
    int num_tunnels;
    struct Tunnel **tunnels;
};

PriorityScheduler *scheduler_create(int num_tunnels, struct Tunnel **tunnels) {
    // TODO
}

void scheduler_destroy(PriorityScheduler *scheduler) {
    // TODO
}

static int get_highest_priority(PriorityScheduler *scheduler) {
    // TODO
}

struct Tunnel *scheduler_admit(PriorityScheduler *scheduler, struct Vehicle *vehicle) {
    // TODO
}

void scheduler_exit(PriorityScheduler *scheduler, struct Vehicle *vehicle) {
    // TODO
}
