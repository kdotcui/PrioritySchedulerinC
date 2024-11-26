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

/**
 * @brief Creates a PriorityScheduler with the given number of tunnels.
 * 
 * @param num_tunnels The number of tunnels.
 * @param tunnels Array of pointers to tunnels.
 * @return Pointer to the created PriorityScheduler.
 */
PriorityScheduler *scheduler_create(int num_tunnels, struct Tunnel **tunnels) {
    PriorityScheduler *scheduler = malloc(sizeof(PriorityScheduler));
    if (!scheduler) {
        perror("scheduler_create: malloc failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the mutex and condition variable
    pthread_mutex_init(&scheduler->lock, NULL);
    pthread_cond_init(&scheduler->lock_cv, NULL);

    // Initialize the priority counts
    for (int i = 0; i <= HIGHEST_PRIORITY; i++) {
        scheduler->priority_counts[i] = 0;
    }

    // Assign the number of tunnels and the tunnels array
    scheduler->num_tunnels = num_tunnels;
    scheduler->tunnels = tunnels;

    // Create the hashmap
    scheduler->tunnel_map = hashmap_create(vehicle_hash);
    if (!scheduler->tunnel_map) {
        perror("scheduler_create: hashmap_create failed");
        free(scheduler);
        exit(EXIT_FAILURE);
    }

    return scheduler;
}

/**
 * @brief Frees the memory associated with the PriorityScheduler.
 * 
 * @param scheduler The PriorityScheduler to destroy.
 */
void scheduler_destroy(PriorityScheduler *scheduler) {
    pthread_mutex_destroy(&scheduler->lock);
    pthread_cond_destroy(&scheduler->lock_cv);
    hashmap_destroy(scheduler->tunnel_map);
    free(scheduler);
}

/**
 * @brief Returns the highest priority with waiting vehicles.
 * 
 * @param scheduler The PriorityScheduler.
 * @return The highest non-zero priority index, or -1 if no vehicles are waiting.
 */
static int get_highest_priority(PriorityScheduler *scheduler) {
    for (int i = HIGHEST_PRIORITY; i >= 0; i--) {
        if (scheduler->priority_counts[i] > 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Admits a vehicle into an available tunnel based on priority.
 * 
 * @param scheduler The PriorityScheduler.
 * @param vehicle The vehicle to admit.
 * @return The tunnel the vehicle was admitted into, or NULL if no tunnel was available.
 */
struct Tunnel *scheduler_admit(PriorityScheduler *scheduler, struct Vehicle *vehicle) {
    pthread_mutex_lock(&scheduler->lock);

    // Increment priority count
    scheduler->priority_counts[vehicle->priority]++;

    // Wait for the highest priority
    while (vehicle->priority != get_highest_priority(scheduler)) {
        pthread_cond_wait(&scheduler->lock_cv, &scheduler->lock);
    }

    // Attempt to find a tunnel
    struct Tunnel *assigned_tunnel = NULL;
    for (int i = 0; i < scheduler->num_tunnels; i++) {
        if (tunnel_try_to_enter(scheduler->tunnels[i], vehicle)) {
            assigned_tunnel = scheduler->tunnels[i];
            hashmap_put(scheduler->tunnel_map, vehicle, assigned_tunnel);
            break;
        }
    }

    // If no tunnel was found, decrement priority and signal others
    if (!assigned_tunnel) {
        scheduler->priority_counts[vehicle->priority]--;
        pthread_cond_broadcast(&scheduler->lock_cv);
    }

    pthread_mutex_unlock(&scheduler->lock);

    return assigned_tunnel;
}

/**
 * @brief Exits a vehicle from its assigned tunnel.
 * 
 * @param scheduler The PriorityScheduler.
 * @param vehicle The vehicle to exit.
 */
void scheduler_exit(PriorityScheduler *scheduler, struct Vehicle *vehicle) {
    pthread_mutex_lock(&scheduler->lock);

    // Find and remove the vehicle from its assigned tunnel
    struct Tunnel *tunnel = hashmap_remove(scheduler->tunnel_map, vehicle);
    if (tunnel) {
        tunnel_exit(tunnel, vehicle);
    }

    scheduler->priority_counts[vehicle->priority]--;
    pthread_cond_broadcast(&scheduler->lock_cv);

    pthread_mutex_unlock(&scheduler->lock);
}
