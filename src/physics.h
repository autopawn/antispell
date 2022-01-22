#ifndef PHYSICS_H
#define PHYSICS_H

#include "level.h"

typedef struct {
    float x, y, vx, vy;
    float rad;
} Body;

void BodyLimitSpeed(Body *body, float mag);

// Retrieves the distance between two bodies (<0 means overlapping)
float BodyDistance(Body body1, Body body2);

// Updates body, retrieves 1 on collision with level
int UpdateBody(const Level *level, Body *body);



#endif
