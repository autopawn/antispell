#include "draw.h"

#include <raylib.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "spell_catalog.h"

static const int TS = LEVEL_TILE_SIZE;

static Texture2D wandTexture[2];
static Texture2D mageTexture[2];
static Texture2D bunnyTexture[2];
static Texture2D flowerTexture[2];
static Texture2D chompTexture[3];
static Texture2D spellTexture;
static Texture2D bubbleTexture[5];
static Texture2D tileTexture[4];

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
    spellTexture = LoadTexture("resources/sprites/spell.png");
    bubbleTexture[0] = LoadTexture("resources/sprites/bubble_angry.png");
    bubbleTexture[1] = LoadTexture("resources/sprites/bubble_look.png");
    bubbleTexture[2] = LoadTexture("resources/sprites/bubble_eat.png");
    bubbleTexture[3] = LoadTexture("resources/sprites/bubble_laugh.png");
    bubbleTexture[4] = LoadTexture("resources/sprites/bubble_crazy.png");
    tileTexture[0] = LoadTexture("resources/tilesets/stairs_Delapouite.png");
    tileTexture[1] = LoadTexture("resources/tilesets/fire-zone_Lorc.png");
    tileTexture[2] = LoadTexture("resources/tilesets/wood-pile_Delapouite.png");
    tileTexture[3] = LoadTexture("resources/tilesets/coral_Delapouite.png");
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
    UnloadTexture(spellTexture);
    UnloadTexture(bubbleTexture[0]);
    UnloadTexture(bubbleTexture[1]);
    UnloadTexture(bubbleTexture[2]);
    UnloadTexture(bubbleTexture[3]);
    UnloadTexture(bubbleTexture[4]);
    UnloadTexture(tileTexture[0]);
    UnloadTexture(tileTexture[1]);
    UnloadTexture(tileTexture[2]);
    UnloadTexture(tileTexture[3]);
}

static Color GetStatusColor(EntityStatus status)
{
    if (status == STATUS_FROZEN) return SKYBLUE;
    if (status == STATUS_ONFIRE) return RED;
    if (status == STATUS_CEO) return DARKGRAY;
    return WHITE;
}

static void DrawParticle(Particle particle)
{
    char symbol[2];
    symbol[0] = particle.character;
    symbol[1] = '\0';
    int fontSize = 2*particle.body.rad;
    int measX = MeasureText(symbol, fontSize);
    Color color = particle.color;
    if (particle.lifeTime % 15 < 4) color = WHITE;

    DrawText(symbol, particle.body.x - measX/2.0, particle.body.y - fontSize/2.0, fontSize, color);
}

static Rectangle textureSrc(const Texture2D *tex){
    return (Rectangle){0, 0, tex->width, tex->height};
}

static Color transp(Color col)
{
    col.a = col.a/3;
    return col;
}

static void DrawLevel(Level *level, DrawLayer layer)
{
    for (int y = 0; y < level->sizeY; y++)
    {
        for (int x = 0; x < level->sizeX; x++)
        {
            char cell = level->cells[y][x];
            Rectangle rect = {x*TS, y*TS, TS, TS};
            Rectangle rect2 = {x*TS+3, y*TS+3, TS-6, TS-6};

            switch (cell)
            {
                case '#':
                {
                    if (layer == LAYER2_ENTS) DrawRectangleRec(rect, BLACK);
                    if (layer == LAYER3_WALLS) DrawRectangleRec(rect, GRAY);
                    break;
                }
                case '~':
                {
                    break;
                }
                case 'l':
                {
                    if (layer == LAYER0_RUG)
                        DrawRectangleRec(rect, RED);
                    if (layer == LAYER1_FLOOR)
                        DrawTexturePro(tileTexture[1], textureSrc(&tileTexture[1]), rect2,
                                (Vector2){0,0}, 0, transp(MAROON));
                    break;
                }
                case 'a':
                {
                    if (layer == LAYER0_RUG)
                        DrawRectangleRec(rect, DARKGRAY);
                    break;
                }
                case 'w':
                {
                    if (layer == LAYER0_RUG)
                        DrawRectangleRec(rect, BROWN);
                    if (layer == LAYER2_ENTS)
                        DrawTexturePro(tileTexture[2], textureSrc(&tileTexture[2]), rect2,
                                (Vector2){0,0}, 0, BROWN);
                    break;
                }
                case 'r':
                {
                    if (layer == LAYER0_RUG)
                        DrawRectangleRec(rect, DARKGREEN);
                    if (layer == LAYER1_FLOOR)
                        DrawTexturePro(tileTexture[3], textureSrc(&tileTexture[3]), rect2,
                                (Vector2){0,0}, 0, transp(DARKGREEN));
                    break;
                }
                case '$':
                {
                    if (layer == LAYER0_RUG)
                        DrawRectangleRec(rect, YELLOW);
                    if (layer == LAYER1_FLOOR)
                        DrawTexturePro(tileTexture[0], textureSrc(&tileTexture[0]), rect2,
                                (Vector2){0,0}, 0, transp(YELLOW));
                    break;
                }

                default:
                {
                    if (layer == LAYER0_RUG)
                    {
                        DrawRectangleRec(rect, BLUE);
                        if (cell != ' '){
                            char symbol[2];
                            symbol[0] = cell;
                            symbol[1] = '\0';
                            int txtWidth = MeasureText(symbol, TS);
                            DrawText(symbol, rect.x + (TS - txtWidth)/2.0, rect.y, TS, YELLOW);
                        }
                    }
                    break;
                }
            }
        }
    }
}

static int isWhite(Color col){
    return col.r == 255 && col.g == 255 && col.b == 255 && col.a == 255;
}

static void DrawStateParticles(State *state, int above)
{
    for (int k = 0; k < state->particlesN; k++)
    {
        if (state->particles[k].above != above) continue;
        DrawParticle(state->particles[k]);
    }
    for (int i = 0; i < state->entsN; i++)
    {
        for (int k = 0; k < state->ents[i].particlesN; k++)
        {
            Particle part = state->ents[i].particles[k];
            if (part.above != above) continue;
            part.body.x += state->ents[i].body.x;
            part.body.y += state->ents[i].body.y;
            DrawParticle(part);
        }
    }
}

void DrawState(State *state, DrawLayer layer){
    if (layer == LAYER2_ENTS)
        DrawStateParticles(state, 0);

    // Draw ents
    for (int i = 0; i < state->entsN; i++)
    {
        const Entity *ent = &state->ents[i];
        if (layer == LAYER0_RUG)
        {
            DrawCircle(ent->body.x, ent->body.y, ent->body.rad, (Color){0, 0, 0, 128});
            if (ent->type == TYPE_CHOMP && ent->status != STATUS_FREE)
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
                    Color col1 = GetPowerCharColor(ent->powerChar);
                    Color col2 = col1;
                    col2.a = 64;
                    DrawCircleGradient(ent->body.x, ent->body.y, ent->body.rad, col1, col2);
                    break;
                }
                case TYPE_SPELL:
                {
                    Rectangle src = {0, 0, spellTexture.width, spellTexture.height};
                    Rectangle dst = {ent->body.x, ent->body.y, 2*ent->body.rad, 2*ent->body.rad};
                    Vector2 origin = {dst.width/2, dst.height/2};
                    DrawTexturePro(spellTexture, src, dst, origin, 0, ent->spell.color);
                    break;
                }
                case TYPE_PLAYER:
                case TYPE_MAGE:
                case TYPE_FLOWER:
                {
                    float rotation = atan2((ent->lookY - ent->body.y), ent->lookX - ent->body.x);
                    rotation = (rotation/M_PI)*180;

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

                    Rectangle src = {0, 0, texture[0].width, texture[0].height};
                    Rectangle dst = {ent->body.x, ent->body.y, 3*ent->body.rad, 3*ent->body.rad};
                    Vector2 origin = {dst.width/2, dst.height/2};

                    Color statusColor = GetStatusColor(ent->status);
                    Color entColor = isWhite(statusColor)? GetPowerCharColor(ent->powerChar) : statusColor;

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

                    if (ent->status != STATUS_FREE)
                    {
                        DrawTexturePro(chompTexture[2], src, dst1, origin, 0, WHITE);
                        DrawLineEx((Vector2){ent->initialBody.x, ent->initialBody.y},
                            (Vector2){ent->body.x, ent->body.y}, 5, entColor);
                    }
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
        if (layer == LAYER3_WALLS)
        {
            int textureId = -1;
            if (ent->status == STATUS_ANGRY)      textureId = 0;
            if (ent->status == STATUS_ASTONISHED) textureId = 1;
            if (ent->status == STATUS_YUMMY)      textureId = 2;
            if (ent->status == STATUS_LAUGH)      textureId = 3;
            if (ent->status == STATUS_CRAZY)      textureId = 4;
            if (textureId >= 0)
            {
                DrawTexture(bubbleTexture[textureId], ent->body.x + ent->body.rad/2,
                       ent->body.y - bubbleTexture[textureId].height - ent->body.rad/2, WHITE);
            }
        }
    }

    if (layer == LAYER2_ENTS)
        DrawStateParticles(state, 1);

    // Draw level
    DrawLevel(state->level, layer);
}

void DrawGUI(State *state)
{
    Entity *player = StateGetPlayer(state);

    float wandX = (GetScreenWidth() - wandTexture[0].width)/2.0;

    Spell spell = state->wand.spell;
    Color spellColor = spell.color;

    int spellValid = spell.type != SPELLTYPE_NONE;

    char symbol[2];
    int spellLength = strlen(state->wand.text);

    Color color = spellValid? spellColor : DARKGRAY;
    Color signalColor = DARKGRAY;
    if (state->wand.signal == WANDSIGNAL_BACKSPACE)
        signalColor = RED;
    if (state->wand.signal == WANDSIGNAL_ABSORB)
        signalColor = GetPowerCharColor(state->wand.absorbingChar);
    if (state->wand.signal == WANDSIGNAL_ABSORBED)
        signalColor = YELLOW;
    if (state->wand.signal == WANDSIGNAL_FULL)
        signalColor = DARKGRAY;
    if (state->wand.signal == WANDSIGNAL_SPELL)
        signalColor = WHITE;
    float si = state->wand.signalIntensity;
    color.r = (unsigned char)(signalColor.r*si + color.r*(1.0 - si));
    color.g = (unsigned char)(signalColor.g*si + color.g*(1.0 - si));
    color.b = (unsigned char)(signalColor.b*si + color.b*(1.0 - si));

    DrawTexture(wandTexture[0], wandX, -25, WHITE);
    DrawTexture(wandTexture[1], wandX, -25, color);

    for (int i = 0; i < MAX_SPELL_LENGHT; i++)
    {
        symbol[0] = (i < spellLength)? state->wand.text[i] : '-';
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

    if (player)
    {
        if (player->coins)
        {
            char coinss[100];
            sprintf(coinss, "$ %d", player->coins);
            int measX = MeasureText(coinss, 32);
            DrawText(coinss, GetScreenWidth() - measX - 4 - 2, GetScreenHeight() - 36 + 2, 32, BLACK);
            DrawText(coinss, GetScreenWidth() - measX - 4, GetScreenHeight() - 36, 32, YELLOW);
        }
    }
}
