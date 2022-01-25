#include "physics.h"

#include <math.h>

static const float SQRT2INV = 1.0/1.41421356237;
static const int TS = LEVEL_TILE_SIZE;

static float signF(float v)
{
    return (v > 0) - (v < 0);
}

float BodyDistance(Body body1, Body body2){
    float dx = body1.x - body2.x;
    float dy = body1.y - body2.y;
    return sqrtf(dx*dx + dy*dy) - body1.rad - body2.rad;
}

static int BodyCollidesWithLevel(const Level *level, Body body)
{
    int xi = (int)((body.x - SQRT2INV*body.rad)/TS);
    int yi = (int)((body.y - SQRT2INV*body.rad)/TS);
    int xf = (int)((body.x + SQRT2INV*body.rad)/TS) + 1;
    int yf = (int)((body.y + SQRT2INV*body.rad)/TS) + 1;
    if (xi < 0) xi = 0;
    if (yi < 0) yi = 0;
    if (xf > level->sizeX) xf = level->sizeX;
    if (yf > level->sizeY) yf = level->sizeY;

    for (int y = yi; y < yf; y++)
    {
        for (int x = xi; x < xf; x++)
        {
            if (LevelCellIsSolid(level->cells[y][x])) return 1;
        }
    }
    return 0;

}

static int moveBody(const Level *level, Body *body, float remX, float remY)
{
    if (remX == 0 && remY == 0) return 0;

    Body next = *body;
    next.x += remX;
    next.y += remY;
    if (!BodyCollidesWithLevel(level, next))
    {
        *body = next;
        return 0;
    }

    // Close the gap with the wall
    float stepX = remX/2.0;
    float stepY = remY/2.0;
    while ((fabsf(stepX) >= 0.25) || (fabsf(stepY) >= 0.25))
    {
        next = *body;
        next.x += stepX;
        next.y += stepY;
        if (!BodyCollidesWithLevel(level, next))
        {
            *body = next;
            remX -= stepX;
            remY -= stepY;
        }
        stepX /= 2.0;
        stepY /= 2.0;
    }
    // Cancel speed in X
    if (body->vx != 0)
    {
        next = *body;
        next.x += signF(body->vx);
        if (BodyCollidesWithLevel(level, next))
        {
            body->vx = 0;
            moveBody(level, body, 0, remY);
            return 1;
        }
    }
    // Cancel speed in Y
    if (body->vy != 0)
    {
        next = *body;
        next.y += signF(body->vy);
        if (BodyCollidesWithLevel(level, next))
        {
            body->vy = 0;
            moveBody(level, body, remX, 0);
            return 1;
        }
    }
    return 1;
}

int UpdateBody(const Level *level, Body *body)
{
    if (!level)
    {
        body->x += body->vx;
        body->y += body->vy;
        return 0;
    }
    return moveBody(level, body, body->vx, body->vy);
}

// Limits the vector to a given magnitude
void LimitVector(float *x, float *y, float mag)
{
    float cmag = sqrtf( (*x)*(*x) + (*y)*(*y));
    if (cmag > mag)
    {
        *x *= mag / cmag;
        *y *= mag / cmag;
    }
}

void BodyLimitSpeed(Body *body, float mag){
    LimitVector(&body->vx, &body->vy, mag);
}

void BodyAccelTowards(Body *body, float tgtX, float tgtY, float accel, float maxSpeed)
{
    float dx = tgtX - body->x;
    float dy = tgtY - body->y;
    float dmag = sqrtf( dx*dx + dy*dy );
    if (dmag == 0) return;
    body->vx += accel*dx/dmag;
    body->vy += accel*dy/dmag;

    BodyLimitSpeed(body, maxSpeed);
}

int BodySetSpeed(Body *body, float mag){
    float cmag = sqrtf( body->vx*body->vx + body->vy*body->vy);
    if (cmag <= 0) return 0;
    body->vx *= mag / cmag;
    body->vy *= mag / cmag;
    return 1;
}

// Moves to the next cell, returns 0 if already in target's cell.
static int cellStepXY(float *posX, float *posY, float tgtX, float tgtY)
{
    // Direction
    int dirX = tgtX > (*posX);
    int dirY = tgtY > (*posY);
    // Current cell
    int cellX = (*posX)/TS;
    int cellY = (*posY)/TS;
    // Check if already in target cell
    int tgtCellX = tgtX/TS;
    int tgtCellY = tgtY/TS;
    if (cellX == tgtCellX && cellY == tgtCellY) return 0;
    // Remaining movement within the cell
    float remX = (dirX > 0)? (cellX + 1)*TS + 0.25 - (*posX) : cellX*TS - 0.25 - (*posX);
    float remY = (dirY > 0)? (cellY + 1)*TS + 0.25 - (*posY) : cellY*TS - 0.25 - (*posY);
    // Check which movement will be performed first
    if (cellX != tgtCellX && fabsf(remX*(tgtY - (*posY))) < fabsf(remY*(tgtX - (*posX))))
        *posX += remX;
    else
        *posY += remY;
    return 1;
}

int LineOfSight(const Level *level, Body body1, Body body2)
{
    // Current position of the ray
    float rayX, rayY;
    rayX = body1.x;
    rayY = body1.y;
    do
    {
        // Current cell of the ray
        int cellX = rayX/TS;
        int cellY = rayY/TS;
        if(rayX < 0 || cellX >= level->sizeX) return 0;
        if(rayY < 0 || cellY >= level->sizeY) return 0;
        // Check if there is an intersection with the current cell
        if (LevelCellIsSolid(level->cells[cellY][cellX]))
            return 0;
    }while(cellStepXY(&rayX, &rayY, body2.x, body2.y));
    // No object blocking
    return 1;
}
