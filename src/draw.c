#include "draw.h"

#include <raylib.h>
#include <stdlib.h>
#include <string.h>

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


static void DrawLevel(Level *level, DrawLayer layer)
{
    for (int y = 0; y < level->sizeY; y++)
    {
        for (int x = 0; x < level->sizeX; x++)
        {
            char cell = level->cells[y][x];
            Rectangle rect = {x*TS, y*TS, TS, TS};

            if (layer == LAYER0_RUG && !LevelCellIsSolid(cell))
            {
                switch (cell)
                {
                    case 'l': DrawRectangleRec(rect, RED); break;

                    default:
                    {
                        DrawRectangleRec(rect, BLUE);
                        if (cell != ' '){
                            char symbol[2];
                            symbol[0] = cell;
                            symbol[1] = '\0';
                            int txtWidth = MeasureText(symbol, TS);
                            DrawText(symbol, rect.x + (TS - txtWidth)/2.0, rect.y, TS, YELLOW);
                        }
                        break;
                    }
                }
            }
            if (cell == '#' && layer == LAYER2_ENTS) DrawRectangleRec(rect, BLACK);
            if (cell == '#' && layer == LAYER3_WALLS) DrawRectangleRec(rect, GRAY);
        }
    }
}


void DrawState(State *state, DrawLayer layer){
    // Draw level
    DrawLevel(state->level, layer);

    // Draw ents
    for (int i = 0; i < state->entsN; i++)
    {
        const Entity *ent = &state->ents[i];
        if (layer == LAYER0_RUG)
        {
            DrawCircle(ent->body.x, ent->body.y, ent->body.rad, (Color){0, 0, 0, 128});
        }
        if (layer == LAYER2_ENTS)
        {
            char symbol[2];
            symbol[1] = '\0';

            if (ent->type == TYPE_PROJECTILE)
            {
                DrawCircle(ent->body.x, ent->body.y, ent->body.rad, YELLOW);
                symbol[0] = (char) ent->powerChar;
                DrawText(symbol, ent->body.x - 8, ent->body.y - 8, 16, ORANGE);
                continue;
            }

            DrawCircle(ent->body.x, ent->body.y, ent->body.rad, RED);
            symbol[0] = (char) ent->type;
            DrawText(symbol, ent->body.x - 8, ent->body.y - 8, 16, ORANGE);
        }

    }

}

void DrawGUI(State *state)
{
    float wandX = (GetScreenWidth() - wandTexture[0].width)/2.0;

    char symbol[2];
    int spellLength = strlen(state->wand.spell);

    Color color = DARKGRAY;
    if (state->wand.signal == WANDSIGNAL_BACKSPACE) color = RED;
    if (state->wand.signal == WANDSIGNAL_ABSORB) color = SKYBLUE;
    if (state->wand.signal == WANDSIGNAL_ABSORBED) color = YELLOW;
    if (state->wand.signal == WANDSIGNAL_FULL) color = DARKBLUE;
    float si = state->wand.signalIntensity;
    color.r = (unsigned char)(color.r*si + DARKGRAY.r*(1.0 - si));
    color.g = (unsigned char)(color.g*si + DARKGRAY.g*(1.0 - si));
    color.b = (unsigned char)(color.b*si + DARKGRAY.b*(1.0 - si));

    DrawTexture(wandTexture[0], wandX, -25, WHITE);
    DrawTexture(wandTexture[1], wandX, -25, color);

    for (int i = 0; i < MAX_SPELL_LENGHT; i++)
    {
        symbol[0] = (i < spellLength)? state->wand.spell[i] : '-';
        symbol[1] = '\0';
        float symbX = wandX + (i+1)*wandTexture[0].width/(MAX_SPELL_LENGHT + 2.5);
        DrawText(symbol, symbX-2, 12, 32, BLACK);
        DrawText(symbol, symbX, 10, 32, WHITE);
    }

    if (state->wand.absorbingTime > 0)
    {
        symbol[0] = state->wand.absorbingChar;
        symbol[1] = '\0';
        float symbX = wandX + (MAX_SPELL_LENGHT + 1.3)*wandTexture[0].width/(MAX_SPELL_LENGHT + 2.5);
        float movX = rand()%7 - 3;
        float movY = rand()%7 - 3;
        DrawText(symbol, symbX - 2 + movX, 12 + movY, 32, BLACK);
        DrawText(symbol, symbX + movX, 10 + movY, 32, color);
    }
}
