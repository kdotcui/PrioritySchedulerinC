#ifndef TUNNEL_H 
#define TUNNEL_H

#include <stdbool.h>
#include "vehicle.h"

extern const int tunnel_capacities[];

typedef struct Log Log;

struct Tunnel {
    int id;
    enum VehicleType vehicle_type; 
    int direction;
    int num_vehicles;
    Log *log;
};

struct Tunnel **tunnels_create(int num_tunnels, Log *log);
void            tunnels_destroy(struct Tunnel **tunnels);

bool            tunnel_try_to_enter(struct Tunnel *tunnel, struct Vehicle *vehicle);
void            tunnel_exit(struct Tunnel *tunnel, struct Vehicle *vehicle);

#endif
