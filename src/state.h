#ifndef STATE_H
#define STATE_H

#include "level.h"

typedef struct {
    float x, y, vx, vy;
    float rad;
} ObjectPhy;

typedef struct {
    Level *level;
} State;

State *StateLoadFromFile(const char *fname);

void StateFree(State *state);


#endif

