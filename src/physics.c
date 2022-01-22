#include "physics.h"

#include <math.h>

static const float SQRT2INV = 1.0/1.41421356237;
static const int TS = LEVEL_TILE_SIZE;

static float signF(float v)
{
    return (v > 0) - (v < 0);
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
    return moveBody(level, body, body->vx, body->vy);
}
