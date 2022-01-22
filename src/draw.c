#include "draw.h"

#include <raylib.h>

static const int TS = LEVEL_TILE_SIZE;

static Texture2D wandTexture[2];

void DrawLoadResources()
{
    wandTexture[0] = LoadTexture("resources/gui/wand0.png");
    wandTexture[1] = LoadTexture("resources/gui/wand1.png");
}

void DrawUnloadResources()
{
    UnloadTexture(wandTexture[0]);
    UnloadTexture(wandTexture[1]);
}


static void DrawLevel(Level *level, int hlev)
{
    for (int y = 0; y < level->sizeY; y++)
    {
        for (int x = 0; x < level->sizeX; x++)
        {
            char cell = level->cells[y][x];
            Rectangle rect = {x*TS, y*TS, TS, TS};

            if (hlev == 0 && !LevelCellIsSolid(cell))
            {
                DrawRectangleRec(rect, PINK);
                if (cell != ' '){
                    char symbol[2];
                    symbol[0] = cell;
                    symbol[1] = '\0';
                    DrawText(symbol, rect.x, rect.y, TS, WHITE);
                }
            }
            if (cell == '#' && hlev == 2) DrawRectangleRec(rect, GRAY);
        }
    }
}


void DrawState(State *state, int hlev){
    // Draw level
    DrawLevel(state->level, hlev);

    // Draw ents
    for (int i = 0; i < state->entsN; i++)
    {
        const Entity *ent = &state->ents[i];
        if (hlev == 0)
        {
            DrawCircle(ent->body.x, ent->body.y, ent->body.rad, DARKGRAY);
        }
        if (hlev == 1)
        {
            DrawCircle(ent->body.x, ent->body.y, ent->body.rad, RED);

            char symbol[2];
            symbol[0] = (char) ent->type;
            symbol[1] = '\0';
            DrawText(symbol, ent->body.x - 8, ent->body.y - 8, 16, ORANGE);
        }

    }

}

void DrawGUI(State *state)
{
    float wand_x = (GetScreenWidth() - wandTexture[0].width)/2.0;
    DrawTexture(wandTexture[0], wand_x, 0, WHITE);
    DrawTexture(wandTexture[1], wand_x, 0, RED);
}
