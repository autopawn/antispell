#ifndef STATE_H
#define STATE_H

#include "level.h"
#include "physics.h"

typedef enum {
    TYPE_PLAYER = '@',
} EntityType;

typedef struct {
    Body body;
    EntityType type;
} Entity;

typedef struct {
    Level *level;

    int entsCapacity;
    int entsN;
    Entity *ents;
} State;


State *StateLoadFromFile(const char *fname);

State *StateCopy(const State *state);

void StateFree(State *state);

void StateUpdate(State *state);

const Entity *StateGetPlayer(const State *state);



#endif

