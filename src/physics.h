#ifndef PHYSICS_H
#define PHYSICS_H

#include "level.h"

typedef struct {
    float x, y, vx, vy;
    float rad;
} Body;

// Updates body, retrieves 1 on collision with level
int UpdateBody(const Level *level, Body *body);


#endif
