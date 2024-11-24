#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include <pthread.h>
#include "tunnel.h"
#include "hashmap.h"
#include "logger.h"

typedef struct PriorityScheduler PriorityScheduler;

PriorityScheduler  *scheduler_create(int num_tunnels, struct Tunnel **tunnels);
void                scheduler_destroy(PriorityScheduler *scheduler);

struct Tunnel      *scheduler_admit(PriorityScheduler *scheduler, struct Vehicle *vehicle);
void                scheduler_exit(PriorityScheduler *scheduler, struct Vehicle *vehicle);

#endif
