#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdlib.h>

#define HIGHEST_PRIORITY 4

enum VehicleType {
    CAR,
    SLED,
    NUM_VEHICLE_TYPES,
};

enum Direction {
    NORTH,
    SOUTH,
    NUM_DIRECTIONS,
};

typedef struct PriorityScheduler PriorityScheduler;

struct Vehicle {
    int id;
    enum VehicleType vehicle_type;
    enum Direction direction;
    int speed;
    int priority;
    PriorityScheduler *scheduler;
};

struct Vehicle *vehicle_create(enum VehicleType type, enum Direction direction, int priority, PriorityScheduler *scheduler);
struct Vehicle *vehicle_random(PriorityScheduler *scheduler);

void *run(void *arg);

size_t vehicle_hash(const struct Vehicle *vehicle);

#endif
