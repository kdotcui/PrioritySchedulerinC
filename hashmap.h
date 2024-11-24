#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include "tunnel.h"
#include "vehicle.h"

typedef struct HashMap HashMap;

HashMap        *hashmap_create(size_t (*hash_func)(const struct Vehicle *key));
void            hashmap_destroy(HashMap *map);

void            hashmap_put(HashMap *map, struct Vehicle *key, struct Tunnel *value);
struct Tunnel  *hashmap_get(HashMap *map, struct Vehicle *key);
struct Tunnel  *hashmap_remove(HashMap *map, struct Vehicle *key);

#endif
