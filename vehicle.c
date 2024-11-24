#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "priority_scheduler.h"
#include "vehicle.h"

const int speeds[] = {
    [CAR] = 6,
    [SLED] = 4,
};

/** @brief  Creates and returns a pointer to a vehicle with the given parameters.
 *  
 *  Each vehicle created has an id (starting at 1) that is one higher than the 
 *  vehicle created by the previous call of this function.
 *  
 *  @param  type        The type of vehicle - determines the vehicle's speed.
 *  @param  direction   The direction of the vehicle.
 *  @param  priority    The priority of the vehicle, in the range 0-4.
 *  @param  scheduler   The scheduler to be used for the vehicle.
 *  @return Pointer to the created vehicle.
 */
struct Vehicle *vehicle_create(enum VehicleType type, enum Direction direction, int priority, PriorityScheduler *scheduler) {
    static int id = 1;
    struct Vehicle *new_vehicle = malloc(sizeof *new_vehicle);
    if (new_vehicle == NULL) {
        perror("vehicle_create");
        exit(EXIT_FAILURE);
    }
    *new_vehicle = (struct Vehicle) { 
        .id = id++,
        .vehicle_type = type, 
        .direction = direction, 
        .priority = priority,
        .speed = speeds[type],
        .scheduler = scheduler,
    };
    return new_vehicle;
}

/** @brief  Creates a vehicle with random type, direction, and priority.
 *
 *  @param  scheduler   The scheduler to be used for the vehicle.
 *  @return Pointer to the created vehicle.
 */
struct Vehicle *vehicle_random(PriorityScheduler *scheduler) {
    return vehicle_create(rand() % NUM_VEHICLE_TYPES, rand() % NUM_DIRECTIONS, rand() % (HIGHEST_PRIORITY + 1), scheduler);
}

/** @brief  Simulates time spent in the tunnel by sleeping for a time based on the vehicle's speed.
 *  
 *  The higher the vehicle's speed, the shorter the sleep time.
 *
 *  @param  vehicle Pointer to the vehicle that has entered a tunnel.
 *  @return Void.
 */
static void do_while_in_tunnel(struct Vehicle *vehicle) {
    struct timespec sleep_time = {0, (10 - vehicle->speed) * 100000000};
    nanosleep(&sleep_time, NULL);
}

/** @brief Find and cross through a tunnel via the scheduler.
 *
 *  When a thread is run, it asks the scheduler to admit it in one
 *  of the tunnels. The scheduler is responsible for taking this vehicle
 *  and going through its collection of available tunnels until it 
 *  succeeds in entering one of them. Then, the vehicle thread will call 
 *  doWhileInTunnel (to simulate doing some work inside the tunnel,
 *  i.e., that it takes time to cross the tunnel) and finally exit that tunnel
 *  through the scheduler.
 *
 *  @param  arg Vehicle struct that will cross a tunnel.
 */
void *run(void *arg) {
    struct Vehicle *vehicle = arg;
    scheduler_admit(vehicle->scheduler, vehicle);
    do_while_in_tunnel(vehicle);
    scheduler_exit(vehicle->scheduler, vehicle);
}

/** @brief  Calculates the hash code of the given vehicle.
 *
 *  @param  vehicle The vehicle to calculate the hash of.
 *  @return The vehicle's hash code.
 */
size_t vehicle_hash(const struct Vehicle *vehicle) {
    size_t hash = 7;
    hash = 23 * hash + vehicle->id;
    hash = 23 * hash + vehicle->direction;
    hash = 23 * hash + vehicle->speed;
    hash = 23 * hash + vehicle->priority;
    return hash;
}
