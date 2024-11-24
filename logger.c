#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"
#include "vehicle.h"
#include "logger.h"

struct Node {
    struct Event *event;
    struct Node *next;
};

struct Log {
    struct Node *head;
    struct Node *tail;
};

const char* const event_strings[] = {
    [ENTER_ATTEMPT] = "trying to enter",
    [ENTER_SUCCESS] = "entered successfully",
    [ENTER_FAILED] = "failed to enter",
    [LEAVE_START] = "leaving",
    [LEAVE_END] = "left",
    [COMPLETE] = "has completed",
    [ERROR] = "error in log",
    [END_TEST] = "end of test",
};

const char* const vehicle_names[] = {
    [CAR] = "CAR",
    [SLED] = "SLED",
};

const char* const direction_strings[] = {
    [NORTH] = "NORTH",
    [SOUTH] = "SOUTH",
};

/** @brief  Creates and returns a pointer to a log.
 *  
 *  You should call `log_destroy` to free the memory allocated to the log.
 *
 *  @return Pointer to the created log.
 */
Log *log_create(void) {
    Log *new_log;
    if ((new_log = malloc(sizeof *new_log)) == NULL) {
        perror("log_create");
        exit(EXIT_FAILURE);
    }
    new_log->head = new_log->tail = NULL;
    return new_log;
}

/** @brief  Frees the memory allocated to the given log.
 *
 *  @param  log The log to destroy.
 *  @return Void.
 */
void log_destroy(Log *log) {
    struct Event *event = log_get_head(log);
    while (event != NULL) {
        free(event);
        event = log_get_head(log);
    }
    free(log);
}

/** @brief  Creates an event with the given attributes.
 *
 *  @param  vehicle     The vehicle involved the event.
 *  @param  tunnel      The tunnel involved the event.
 *  @param  event_type  The type of event.
 *  @return Pointer to the created event.
 */
static struct Event *event_create(struct Vehicle *vehicle, struct Tunnel *tunnel, enum EventType event_type) {
    struct Event *new_event;
    if ((new_event = malloc(sizeof *new_event)) == NULL) {
        perror("event_create");
        exit(EXIT_FAILURE);
    }
    *new_event = (struct Event){ .vehicle = vehicle, .tunnel = tunnel, .event_type = event_type };
    return new_event;
}

/** @brief  Adds an event with the given attributes to the given log.
 *
 *  @param  log         The log to add the event to.
 *  @param  vehicle     The vehicle involved the event.
 *  @param  tunnel      The tunnel involved the event.
 *  @param  event_type  The type of event.
 *  @return Void.
 */
void log_add(Log *log, struct Vehicle *vehicle, struct Tunnel *tunnel, enum EventType event_type) {
    struct Node *new_node;
    if ((new_node = malloc(sizeof *new_node)) == NULL) {
        perror("log_add");
        exit(EXIT_FAILURE);
    }
    struct Event *new_event = event_create(vehicle, tunnel, event_type); 
    new_node->event = new_event;
    if (log->tail == NULL) {
        new_node->next = NULL;
        log->head = log->tail = new_node;
    } else {
        log->tail = log->tail->next = new_node;
    }
}

/** @brief  Retrieves and removes the head of the given log.
 *
 *  @param  log The log to get the event from.
 *  @return The event at the head of the log, or NULL if the log is empty.
 */
struct Event *log_get_head(Log *log) {
    if (log->head == NULL) {
        return NULL;
    }
    struct Event *event = log->head->event;
    struct Node *old_head = log->head;
    log->head = log->head->next;
    free(old_head);
    if (log->head == NULL) {
        log->tail = NULL;
    }
    return event;
}

/** @brief  Prints a description of the given event.
 *
 *  @param  event   The event to be printed.
 *  @return Void.
 */
void print_event(struct Event *event) {
    struct Vehicle *vehicle = event->vehicle;
    printf("%s %s %d with priority %d %s %d\n", direction_strings[vehicle->direction],
            vehicle_names[vehicle->vehicle_type], vehicle->id, vehicle->priority, 
            event_strings[event->event_type], event->tunnel->id);
}
