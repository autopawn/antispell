#include "draw.h"

#include <raylib.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spell_catalog.h"

static const int TS = LEVEL_TILE_SIZE;

static Texture2D wandTexture[2];
static Texture2D mageTexture[2];
static Texture2D bunnyTexture[2];
static Texture2D flowerTexture[2];
static Texture2D chompTexture[3];

void DrawLoadResources()
{
    wandTexture[0] = LoadTexture("resources/gui/wand0.png");
    wandTexture[1] = LoadTexture("resources/gui/wand1.png");
    mageTexture[0] = LoadTexture("resources/sprites/mage0.png");
    mageTexture[1] = LoadTexture("resources/sprites/mage1.png");
    bunnyTexture[0] = LoadTexture("resources/sprites/bunny0.png");
    bunnyTexture[1] = LoadTexture("resources/sprites/bunny1.png");
    flowerTexture[0] = LoadTexture("resources/sprites/flower0.png");
    flowerTexture[1] = LoadTexture("resources/sprites/flower1.png");
    chompTexture[0] = LoadTexture("resources/sprites/chomp0.png");
    chompTexture[1] = LoadTexture("resources/sprites/chomp1.png");
    chompTexture[2] = LoadTexture("resources/sprites/base.png");
}

void DrawUnloadResources()
{
    UnloadTexture(wandTexture[0]);
    UnloadTexture(wandTexture[1]);
    UnloadTexture(mageTexture[0]);
    UnloadTexture(mageTexture[1]);
    UnloadTexture(bunnyTexture[0]);
    UnloadTexture(bunnyTexture[1]);
    UnloadTexture(flowerTexture[0]);
    UnloadTexture(flowerTexture[1]);
    UnloadTexture(chompTexture[0]);
    UnloadTexture(chompTexture[1]);
    UnloadTexture(chompTexture[2]);
}

static Color GetStatusColor(EntityStatus status)
{
    if (status == STATUS_FROZEN) return SKYBLUE;
    return WHITE;
}

static Color GetPowerCharColor(char c)
{
    if (c == 0) return WHITE;
    if (c == 'I') return ORANGE;
    if (c == 'C') return BLUE;
    if (c == 'E') return GREEN;
    return WHITE;
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

static int isWhite(Color col){
    return col.r == 255 && col.g == 255 && col.b == 255 && col.a == 255;
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
            if (ent->type == TYPE_CHOMP)
            {
                DrawLineEx((Vector2){ent->initialBody.x, ent->initialBody.y},
                        (Vector2){ent->body.x, ent->body.y}, 5, (Color){0, 0, 0, 128});
                DrawCircle(ent->initialBody.x, ent->initialBody.y, ent->initialBody.rad, (Color){0, 0, 0, 128});
            }
        }
        if (layer == LAYER2_ENTS)
        {
            char symbol[2];
            symbol[1] = '\0';

            switch (ent->type)
            {
                case TYPE_PROJECTILE:
                {
                    DrawCircle(ent->body.x, ent->body.y, ent->body.rad, YELLOW);
                    symbol[0] = (char) ent->powerChar;
                    int textW = MeasureText(symbol, 24);
                    DrawText(symbol, ent->body.x - textW/2.0, ent->body.y - 12, 24, ORANGE);
                    break;
                }
                case TYPE_SPELL:
                {
                    Color spellColor = (Color){
                            ent->spell.color.r, ent->spell.color.g, ent->spell.color.b, ent->spell.color.a};
                    DrawCircle(ent->body.x, ent->body.y, ent->body.rad, spellColor);

                    int spelli = (ent->statusTime/24)%(1+strlen(ent->spell.name));
                    symbol[0] = (char) ent->spell.name[spelli];
                    int textW = MeasureText(symbol, 24);
                    DrawText(symbol, ent->body.x - textW/2.0, ent->body.y - 12, 24, BLACK);
                    break;
                }
                case TYPE_PLAYER:
                case TYPE_MAGE:
                case TYPE_FLOWER:
                {
                    float rotation = atan2((ent->lookY - ent->body.y), ent->lookX - ent->body.x);
                    rotation = (rotation/M_PI)*180;

                    Rectangle src = {0, 0, mageTexture[0].width, mageTexture[0].height};
                    Rectangle dst = {ent->body.x, ent->body.y, 3*ent->body.rad, 3*ent->body.rad};
                    Vector2 origin = {dst.width/2, dst.height/2};

                    Color statusColor = GetStatusColor(ent->status);
                    Color entColor = isWhite(statusColor)? GetPowerCharColor(ent->powerChar) : statusColor;

                    Texture2D texture[2];
                    texture[0] = flowerTexture[0];
                    texture[1] = flowerTexture[1];
                    if (ent->type == TYPE_PLAYER)
                    {
                        texture[0] = bunnyTexture[0];
                        texture[1] = bunnyTexture[1];
                    }
                    if (ent->type == TYPE_MAGE)
                    {
                        texture[0] = mageTexture[0];
                        texture[1] = mageTexture[1];
                    }
                    DrawTexturePro(texture[0], src, dst, origin, rotation, entColor);
                    DrawTexturePro(texture[1], src, dst, origin, rotation, statusColor);
                    break;
                }
                case TYPE_CHOMP:
                {
                    float rotation = atan2((ent->lookY - ent->body.y), ent->lookX - ent->body.x);
                    rotation = (rotation/M_PI)*180;

                    Rectangle src = {0, 0, mageTexture[0].width, mageTexture[0].height};
                    Rectangle dst1 = {ent->initialBody.x, ent->initialBody.y, 3*ent->body.rad, 3*ent->body.rad};
                    Rectangle dst2 = {ent->body.x, ent->body.y, 3*ent->body.rad, 3*ent->body.rad};
                    Vector2 origin = {dst1.width/2, dst1.height/2};

                    Color statusColor = GetStatusColor(ent->status);
                    Color entColor = isWhite(statusColor)? GetPowerCharColor(ent->powerChar) : statusColor;

                    DrawTexturePro(chompTexture[2], src, dst1, origin, 0, WHITE);
                    DrawLineEx((Vector2){ent->initialBody.x, ent->initialBody.y},
                        (Vector2){ent->body.x, ent->body.y}, 5, entColor);
                    DrawTexturePro(chompTexture[0], src, dst2, origin, rotation, entColor);
                    DrawTexturePro(chompTexture[1], src, dst2, origin, rotation, statusColor);
                    break;
                }
                default:
                {
                    DrawCircle(ent->body.x, ent->body.y, ent->body.rad, (ent->status == STATUS_FROZEN)? SKYBLUE : RED);
                    symbol[0] = (char) ent->powerChar;
                    DrawText(symbol, ent->body.x - 8, ent->body.y - 8, 16, ORANGE);
                }
            }
        }
    }
}

void DrawGUI(State *state)
{
    float wandX = (GetScreenWidth() - wandTexture[0].width)/2.0;

    Spell spell = GetSpell(state->wand.spell);
    Color spellColor = (Color){
            .r = spell.color.r, .g = spell.color.g, .b = spell.color.b, .a = spell.color.a};

    int spellValid = spell.type != SPELLTYPE_NONE;

    char symbol[2];
    int spellLength = strlen(state->wand.spell);

    Color color = spellValid? spellColor : DARKGRAY;
    Color signalColor = DARKGRAY;
    if (state->wand.signal == WANDSIGNAL_BACKSPACE) signalColor = RED;
    if (state->wand.signal == WANDSIGNAL_ABSORB) signalColor = SKYBLUE;
    if (state->wand.signal == WANDSIGNAL_ABSORBED) signalColor = YELLOW;
    if (state->wand.signal == WANDSIGNAL_FULL) signalColor = DARKBLUE;
    if (state->wand.signal == WANDSIGNAL_SPELL) signalColor = WHITE;
    float si = state->wand.signalIntensity;
    color.r = (unsigned char)(signalColor.r*si + color.r*(1.0 - si));
    color.g = (unsigned char)(signalColor.g*si + color.g*(1.0 - si));
    color.b = (unsigned char)(signalColor.b*si + color.b*(1.0 - si));

    DrawTexture(wandTexture[0], wandX, -25, WHITE);
    DrawTexture(wandTexture[1], wandX, -25, color);

    for (int i = 0; i < MAX_SPELL_LENGHT; i++)
    {
        symbol[0] = (i < spellLength)? state->wand.spell[i] : '-';
        symbol[1] = '\0';
        float symbX = wandX + (i+1)*wandTexture[0].width/(MAX_SPELL_LENGHT + 2.5);
        DrawText(symbol, symbX-2, 12, 32, BLACK);
        DrawText(symbol, symbX, 10, 32, spellValid? spellColor : WHITE);
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
