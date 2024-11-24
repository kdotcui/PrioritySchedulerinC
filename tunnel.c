#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "logger.h"
#include "vehicle.h"
#include "tunnel.h"

const int tunnel_capacities[] = {
    [CAR] = 3,
    [SLED] = 1,
};

/** @brief  Intitializes and returns an array of pointers to tunnels.
 *  
 *  The array is of size num_tunnels + 1 and is terminated by a NULL value.
 *  All tunnels have the same log and have an id equivalent to their index in the array.
 *  You should call `tunnels_destroy` to free the memory allocated by this function.
 *  
 *  @param  num_tunnels The number of tunnels to create.
 *  @return Pointer to the array of tunnels.
 */
struct Tunnel **tunnels_create(int num_tunnels, Log *log) {
    struct Tunnel **tunnels = malloc((num_tunnels + 1) * sizeof *tunnels);
    if (tunnels == NULL) {
        perror("tunnels_create");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_tunnels; i++) {
        if ((tunnels[i] = malloc(sizeof *tunnels[i])) == NULL) {
            perror("tunnels_create");
            exit(EXIT_FAILURE);
        }
        *tunnels[i] = (struct Tunnel) { .id = i, .num_vehicles = 0, .log = log };
    }
    tunnels[num_tunnels] = NULL;
    return tunnels;
}

/** @brief  Frees the memory allocated by `tunnels_create`.
 *
 *  @param  tunnels The array of tunnels created by `tunnels_create`.
 *  @return Void.
 */
void tunnels_destroy(struct Tunnel **tunnels) {
    for (int i = 0; tunnels[i] != NULL; i++) {
        free(tunnels[i]);
    }
    free(tunnels);
}

/** @brief  Enters the given vehicle into the given tunnel if possible, based on the vehicles
 *          currently in the tunnel.
 *  
 *  Called by `tunnel_try_to_enter`. Vehicle can enter if it is has the same type and direction as the
 *  other vehicles in the tunnel, and the tunnel's capacity has not been reached.
 *  
 *  @param  tunnel  Pointer to the tunnel.
 *  @param  vehicle Pointer to the vehicle attempting to enter.
 *  @return True if the vehicle enters the tunnel successfully, false otherwise.
 */
static bool try_to_enter_inner(struct Tunnel *tunnel, struct Vehicle *vehicle) {
    // TODO
}

/** @brief  Removes a vehicle from the given tunnel.
 *
 *  Called by `tunnel_exit`.
 *
 *  @param  tunnel  Pointer to the tunnel.
 *  @return Void.
 */
static void exit_tunnel_inner(struct Tunnel *tunnel) {
    // TODO
}

/** @brief  Enters the given vehicle into the given tunnel if possible, based on the vehicles
 *          currently in the tunnel.
 *  
 *  Also addes entries for enter attempt and enter result to the tunnel's log.
 *  
 *  @param  tunnel  Pointer to the tunnel.
 *  @param  vehicle Pointer to the vehicle attempting to enter.
 *  @return True if the vehicle enters the tunnel successfully, false otherwise.
 */
bool tunnel_try_to_enter(struct Tunnel *tunnel, struct Vehicle *vehicle) {
    log_add(tunnel->log, vehicle, tunnel, ENTER_ATTEMPT);
    if (try_to_enter_inner(tunnel, vehicle)) {
        log_add(tunnel->log, vehicle, tunnel, ENTER_SUCCESS);
        return true;
    }
    log_add(tunnel->log, vehicle, tunnel, ENTER_FAILED);
    return false;
}

/** @brief  The given vehicle exits the given tunnel.
 *
 *  Also adds appropriate entries to the log.
 *
 *  @param  tunnel  Pointer to the tunnel.
 *  @param  vehicle Pointer to the vehicle exiting.
 *  @return Void.
 */
void tunnel_exit(struct Tunnel *tunnel, struct Vehicle *vehicle) {
    log_add(tunnel->log, vehicle, tunnel, LEAVE_START);
    exit_tunnel_inner(tunnel);
    log_add(tunnel->log, vehicle, tunnel, LEAVE_END);
}
