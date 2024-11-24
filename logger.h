#ifndef LOGGER_H
#define LOGGER_H

#include "tunnel.h"
#include "vehicle.h"

enum EventType {
    ENTER_ATTEMPT,
    ENTER_SUCCESS,
    ENTER_FAILED,
    LEAVE_START,
    LEAVE_END,
    COMPLETE,
    ERROR,
    END_TEST,
    NUM_EVENT_TYPES,
};

typedef struct Log Log;

struct Event {
    struct Vehicle *vehicle;
    struct Tunnel *tunnel;
    enum EventType event_type;
};


Log            *log_create(void);
void            log_destroy(Log *log);

void            log_add(Log *log, struct Vehicle *vehicle, struct Tunnel *tunnel, enum EventType event_type);
struct Event   *log_get_head(Log *log);

void            print_event(struct Event *event);

#endif
