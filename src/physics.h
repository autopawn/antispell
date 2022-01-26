#ifndef PHYSICS_H
#define PHYSICS_H

#include "level.h"

typedef struct {
    float x, y, vx, vy;
    float rad;
} Body;

// Limits the vector to a given magnitude
void LimitVector(float *x, float *y, float mag);

// Limits the body speed to the given magnitude
void BodyLimitSpeed(Body *body, float mag);

// Accel towards a given position
void BodyAccelTowards(Body *body, float tgtX, float tgtY, float accel, float maxSpeed);

// Move towards a given position, set the speed.
void BodyMoveTowards(Body *body, float tgtX, float tgtY, float speed);

// Makes the body speed have the given magnitude, preserving the angle.
// Retrieves 0 ff current speed is 0.
int BodySetSpeed(Body *body, float mag);

// Retrieves the distance between two bodies (<0 means overlapping)
float BodyDistance(Body body1, Body body2);

// Updates body, retrieves 1 on collision with level
int UpdateBody(const Level *level, Body *body);

// Wheter there is a line of sight between body1 and body2
int LineOfSight(const Level *level, Body body1, Body body2);



#endif
