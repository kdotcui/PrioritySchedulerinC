#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vehicle.h"
#include "priority_scheduler.h"
#include "logger.h"
#include "thread.h"
#include "hashmap.h"

struct TunnelState {
    int num_vehicles;
    enum VehicleType vehicle_type;
    enum Direction direction;
};

static void remove_from_tunnel(struct TunnelState *tunnel_state) {
    tunnel_state->num_vehicles--;
}

static void put_in_tunnel(struct TunnelState *tunnel_state, struct Vehicle *vehicle) {
    tunnel_state->num_vehicles++;
    tunnel_state->vehicle_type = vehicle->vehicle_type;
    tunnel_state->direction = vehicle->direction;
}

static bool should_enter(struct TunnelState *tunnel_state, struct Vehicle *vehicle) {
    if (tunnel_state->num_vehicles == 0) {
        return true;
    }
    if (tunnel_state->vehicle_type != vehicle->vehicle_type || tunnel_state->direction != vehicle->direction) {
        return false;
    }
    return tunnel_state->num_vehicles < tunnel_capacities[vehicle->vehicle_type];
}

static void verify_log(Log *log, int num_tunnels, int num_vehicles) {
    struct TunnelState *tunnel_states = calloc(num_tunnels, sizeof *tunnel_states);
    if (tunnel_states == NULL) {
        perror("verify_log");
        exit(EXIT_FAILURE);
    }
    HashMap *tunnel_map = hashmap_create(&vehicle_hash);
    int last_attempt_priority = HIGHEST_PRIORITY;
    int num_enter = 0;
    int num_leave = 0;
    struct Event *current_event = log_get_head(log);
    while (current_event != NULL) {
        struct TunnelState *tunnel_state = &tunnel_states[current_event->tunnel->id];
        switch (current_event->event_type) {
            case ENTER_ATTEMPT:
                if (current_event->vehicle->priority > last_attempt_priority) {
                    print_event(current_event);
                    printf("Vehicle waited for lower priority vehicle\n");
                }
                last_attempt_priority = current_event->vehicle->priority;
                break;
            case ENTER_SUCCESS:
                print_event(current_event);
                num_enter++;
                if (should_enter(tunnel_state, current_event->vehicle)) {
                    put_in_tunnel(tunnel_state, current_event->vehicle);   
                    hashmap_put(tunnel_map, current_event->vehicle, current_event->tunnel);
                } else if (hashmap_get(tunnel_map, current_event->vehicle) != NULL) {
                    printf("Vehicle is already in a tunnel.\n");
                } else {
                    printf("Vehicle should not have entered tunnel.\n");
                }
                break;
            case ENTER_FAILED:
                if (should_enter(tunnel_state, current_event->vehicle)) {
                    print_event(current_event);
                    printf("Vehicle should have entered tunnel.\n");
                }
                break;
            case LEAVE_START:
                break;
            case LEAVE_END:
                print_event(current_event);
                num_leave++;
                if (hashmap_remove(tunnel_map, current_event->vehicle) == NULL) {
                    printf("Vehicle was not in a tunnel.\n");
                } 
                remove_from_tunnel(tunnel_state);
                break;
            case END_TEST:
                break;
            default:
                printf("Error\n");
        }
        free(current_event);
        current_event = log_get_head(log);
    }
    if (num_enter != num_vehicles) {
        printf("Not all %d vehicles entered a tunnel.\n", num_vehicles);
    } else if (num_leave != num_vehicles) {
        printf("Not all %d vehicles left a tunnel.\n", num_vehicles);
    } else {
        printf("All %d vehicles entered and left a tunnel correctly.\n", num_vehicles);
    }
    hashmap_destroy(tunnel_map);
    free(tunnel_states);
}

void run_simulation(int num_tunnels, int num_vehicles) {
    Log *log = log_create();
    struct Tunnel **tunnels = tunnels_create(num_tunnels, log);
    struct PriorityScheduler *scheduler = scheduler_create(num_tunnels, tunnels);
    struct ThreadData threads[num_vehicles];

    for (int i = 0; i < num_vehicles; i++) {
        if (i <= num_tunnels) {
            threads[i].vehicle = vehicle_create(SLED, NORTH, HIGHEST_PRIORITY, scheduler);
        } else {
            threads[i].vehicle = vehicle_random(scheduler);
        }
        thread_start(&threads[i]);
    }
    for (int i = 0; i < num_vehicles; i++) {
        thread_join(&threads[i]);
    }
    verify_log(log, num_tunnels, num_vehicles);
    tunnels_destroy(tunnels);
    log_destroy(log);
    scheduler_destroy(scheduler);
    for (int i = 0; i < num_vehicles; i++) {
        free(threads[i].vehicle);
    }
}

int main(int argc, char **argv) {
    run_simulation(10, 100);
}
