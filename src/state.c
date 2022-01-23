#include "state.h"

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static const float WAND_RADIOUS = 30;
static const int WAND_ABSORV_TIME = 30;

static const int TS = LEVEL_TILE_SIZE;

static void StateAddEntity(State *state, EntityType type, Body body)
{
    // Extend vector capacity
    if (state->entsN == state->entsCapacity)
    {
        state->entsCapacity *= 2;
        state->ents = realloc(state->ents, sizeof(Entity) * state->entsCapacity);
        assert(state->ents != NULL);
    }

    Entity *ent = &state->ents[state->entsN];
    state->entsN++;
    memset(ent, 0, sizeof(Entity));
    ent->type = type;
    ent->body = body;
    ent->body.rad = 0.25*TS;

    switch (ent->type)
    {
        case TYPE_PLANT:
        {
            ent->powerChar = 'I';
            break;
        }

        default:
        {
            break;
        }
    }
}

State *StateLoadFromFile(const char *fname)
{
    State *state = malloc(sizeof(State));
    assert(state != NULL);
    memset(state, 0, sizeof(*state));
    state->level = LevelLoadFromFile(fname);

    state->entsCapacity = 4;
    state->ents = malloc(sizeof(Entity) * state->entsCapacity);
    assert(state->ents != NULL);
    state->entsN = 0;

    // Add entities from the map
    for (int y = 0; y < state->level->sizeY; y++)
    {
        for (int x = 0; x < state->level->sizeX; x++)
        {
            char cell = state->level->cells[y][x];
            Body body = {(x + 0.5)*TS, (y + 0.5)*TS, 0, 0};

            if (cell=='@' || ('A' <= cell && cell <= 'Z'))
                StateAddEntity(state, cell, body);
        }
    }

    return state;
}

State *StateCopy(const State *state1)
{
    State *state2 = malloc(sizeof(State));
    assert(state2 != NULL);
    state2->level = LevelCopy(state1->level);

    state2->entsN = state1->entsN;
    state2->ents = malloc(sizeof(Entity) * state1->entsN);
    assert(state2->ents != NULL);
    memcpy(state2->ents, state1->ents, sizeof(Entity) * state1->entsN);

    return state2;
}

void StateFree(State *state)
{
    free(state->ents);
    LevelFree(state->level);
    free(state);
}

Entity *StateGetPlayer(const State *state){
    Entity *ent = NULL;
    for (int i = 0; i < state->entsN; i++)
    {
        if (state->ents[i].type == TYPE_PLAYER)
            ent = &state->ents[i];
    }
    return ent;
}

static void StateUpdateEntity(State *state, Entity *ent, int colliding, int process_pressed_keys)
{
    switch (ent->type)
    {
        case TYPE_PLAYER:
        {
            // Update player controls
            if (IsKeyDown(KEY_A)) ent->body.vx -= 0.1;
            if (IsKeyDown(KEY_D)) ent->body.vx += 0.1;
            if (IsKeyDown(KEY_W)) ent->body.vy -= 0.1;
            if (IsKeyDown(KEY_S)) ent->body.vy += 0.1;
            BodyLimitSpeed(&ent->body, 2.0);
            if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))
                ent->body.vx *= 0.6;
            if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
                ent->body.vy *= 0.6;

            // Look at the mouse
            float lookDeltaX = GetMouseX() - GetScreenWidth()/2.0;
            float lookDeltaY = GetMouseY() - GetScreenHeight()/2.0;
            ent->lookX = ent->body.x + lookDeltaX/2.0;
            ent->lookY = ent->body.y + lookDeltaY/2.0;

            if (process_pressed_keys && IsKeyPressed(KEY_BACKSPACE) && state->wand.spell[0] != '\0')
            {
                state->wand.spell[strlen(state->wand.spell) - 1] = '\0';
                state->wand.signal = WANDSIGNAL_BACKSPACE;
                state->wand.signalIntensity = 1.0;
            }
        } break;

        case 'C':
        {
            const Entity *player = StateGetPlayer(state);
            printf("Watching %d\n", LineOfSight(state->level, ent->body, player->body));
        }

        default:
        {
            break;
        }
    }
}


void StateUpdate(State *state, int process_pressed_keys)
{
    { // Wand update
        state->wand.signalIntensity *= 0.96;

        int spellLen = strlen(state->wand.spell);
        if (state->wand.absorbingTime >= WAND_ABSORV_TIME && spellLen < MAX_SPELL_LENGHT)
        {
            state->wand.spell[spellLen] = state->wand.absorbingChar;
            state->wand.spell[spellLen + 1] = '\0';
            state->wand.absorbingChar = 0;
            state->wand.absorbingTime = 0;
            state->wand.signal = WANDSIGNAL_ABSORBED;
            state->wand.signalIntensity = 1.0;
            spellLen++;
        }

        Entity *player = StateGetPlayer(state);
        int absorving = 0;
        if (player)
        {
            for (int i = 0; i < state->entsN; i++)
            {
                Entity *ent = &state->ents[i];
                if (ent->powerChar == 0)
                    continue;
                if (state->wand.absorbingChar != 0 && state->wand.absorbingChar != ent->powerChar)
                    continue;
                if (BodyDistance(player->body, ent->body) <= WAND_RADIOUS)
                {
                    absorving = 1;
                    state->wand.absorbingChar = ent->powerChar;
                }
            }
        }
        if (absorving)
        {
            state->wand.absorbingTime++;
            if (spellLen == MAX_SPELL_LENGHT)
            {
                state->wand.absorbingTime = 1;
                if (state->wand.signalIntensity < 0.5 || state->wand.signal == WANDSIGNAL_FULL)
                {
                    state->wand.signal = WANDSIGNAL_FULL;
                    state->wand.signalIntensity = 1.0;
                }
            }
            else
            {
                if (state->wand.signalIntensity < 0.5 || state->wand.signal == WANDSIGNAL_ABSORB)
                {
                    state->wand.signal = WANDSIGNAL_ABSORB;
                    state->wand.signalIntensity = 1.0;
                }
            }

        }
        else
        {
            state->wand.absorbingChar = 0;
            state->wand.absorbingTime = 0;
        }

    }

    // Entities update
    for (int i = 0; i < state->entsN; i++)
    {
        Entity *ent = &state->ents[i];
        int colliding = UpdateBody(state->level, &ent->body);
        StateUpdateEntity(state, ent, colliding, process_pressed_keys);
    }
}
