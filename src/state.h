#ifndef STATE_H
#define STATE_H

#include "level.h"

#define LEVEL_TILE_SIZE 50

typedef struct {
    float x, y, vx, vy;
    float rad;
} Body;

typedef enum {
    TYPE_PLAYER = '@',
} EntityType;

typedef struct {
    Body body;
    EntityType type;
} Entity;

typedef struct {
    Level *level;

    int entsN;
    Entity *ents;
} State;

State *StateLoadFromFile(const char *fname);

State *StateCopy(const State *state);

void StateUpdate(const State *state);

void StateFree(State *state);


#endif

