#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"
#include "vehicle.h"
#include "hashmap.h"

#define DEFAULT_CAPACITY 64

struct Node {
    struct Vehicle *key;
    struct Tunnel *value;
    struct Node *next;
};

struct HashMap {
    size_t (*hash_func)(const struct Vehicle *key);
    struct Node **buckets;
    size_t size;
    size_t capacity;
};

/** @brief  Creates and returns a pointer to a hashmap with the given capacity.
 *
 *  You should call `hashmap_destroy` to free the memory allocated to the hashmap.
 *
 *  @param  hash_func   Function pointer for the hash function to be used in the map.
 *  @return Pointer to the created hashmap.
 */
HashMap *hashmap_create(size_t (*hash_func)(const struct Vehicle *key)) {
    HashMap *map = malloc(sizeof *map);
    if (map == NULL) {
        perror("hashmap_create");
        exit(EXIT_FAILURE);
    }
    map->buckets = calloc(DEFAULT_CAPACITY, sizeof *map->buckets);
    if (map->buckets == NULL) {
        perror("hashmap_create");
        free(map);
        exit(EXIT_FAILURE);
    }
    map->hash_func = hash_func;
    map->capacity = DEFAULT_CAPACITY;
    map->size = 0;
    return map;
}

/** @brief  Frees the memory allocated to the given hashmap.
 *
 *  @param  map The hashmap to destroy.
 *  @return Void.
 */
void hashmap_destroy(HashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        struct Node *node = map->buckets[i];
        while (node != NULL) {
            struct Node *tmp = node;
            node = node->next;
            free(tmp);
        }
    }
    free(map->buckets);
    free(map);
}

/** @brief  Creates and returns a node to hold the given key and value.
 *  
 *  @param  key     The vehicle with which the given tunnel is associated.
 *  @param  value   The tunnel associated with the given key.
 *  @return Pointer to the created node.
 */
static struct Node *create_node(struct Vehicle *key, struct Tunnel *value) {
    struct Node *node = malloc(sizeof *node);
    if (node == NULL) {
        perror("hashmap: create_node");
        exit(EXIT_FAILURE);
    }
    *node = (struct Node){ .key = key, .value = value, .next = NULL };
    return node;
}

/** @brief  Puts the key value pair into the given hashmap.
 *  
 *  @param  map     The hashmap to put into.
 *  @param  key     The vehicle with which the given tunnel is to be associated.
 *  @param  value   The tunnel to be associated with the given key.
 *  @return Void. 
 */
void hashmap_put(HashMap *map, struct Vehicle *key, struct Tunnel *value) {
    size_t index = map->hash_func(key) % map->capacity;
    struct Node *node = map->buckets[index];
    while (node != NULL) {
        if (key == node->key) {
            node->value = value;
            return;
        }
        node = node->next;
    }
    struct Node *new_node = create_node(key, value);
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;
}

/** @brief  Retrieves the tunnel associated with the given vehicle.
 *
 *  @param  map The hashmap to get from.
 *  @param  key The key whose associated tunnel is to be retrieved.
 *  @return The tunnel associated to the given vehicle, or NULL if there is no mapping for the given key.
 */
struct Tunnel *hashmap_get(HashMap *map, struct Vehicle *key) {
    size_t index = map->hash_func(key) % map->capacity;
    struct Node *node = map->buckets[index];
    while (node != NULL) {
        if (key == node->key) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

/** @brief  Retrieves and removes the tunnel associated with the given vehicles.
 *
 *  @param  map The hashmap to remove from.
 *  @param  key The key whose associated tunnel is to be removed.
 *  @return The tunnel associated to the given vehicle, or NULL if there was no mapping for the given key.
 */
struct Tunnel *hashmap_remove(HashMap *map, struct Vehicle *key) {
    size_t index = map->hash_func(key) % map->capacity;
    struct Node *node = map->buckets[index];
    struct Node *prev = NULL;
    while (node != NULL) {
        if (key == node->key) {
            struct Tunnel *tunnel = node->value;
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                map->buckets[index] = node->next;
            }
            free(node);
            map->size--;
            return tunnel;
        }
        prev = node;
        node = node->next;
    }
    return NULL;
}
