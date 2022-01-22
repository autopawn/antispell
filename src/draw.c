#include "draw.h"

#include <raylib.h>

static const int TS = LEVEL_TILE_SIZE;

static void DrawLevel(Level *level, int hlev)
{
    for (int y = 0; y < level->sizeY; y++)
    {
        for (int x = 0; x < level->sizeX; x++)
        {
            char cell = level->cells[y][x];
            Rectangle rect = {x*TS, y*TS, TS, TS};

            if (cell == ' ' && hlev == 0) DrawRectangleRec(rect, PINK);
            if (cell == '#' && hlev == 1) DrawRectangleRec(rect, GRAY);
        }
    }
}


void DrawState(State *state, int hlev){
    // Draw level
    DrawLevel(state->level, hlev);

}
