#include "state.h"

#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
        case TYPE_PLAYER:
        {
            break;
        }
    }
}

static void StateUpdateEntity(State *state, Entity *ent, int colliding)
{
    switch (ent->type)
    {
        case TYPE_PLAYER:
        {
            // Update player controls
            if (IsKeyDown(KEY_RIGHT)) ent->body.vx += 0.1;
            if (IsKeyDown(KEY_LEFT))  ent->body.vx -= 0.1;
            if (IsKeyDown(KEY_UP))    ent->body.vy -= 0.1;
            if (IsKeyDown(KEY_DOWN))  ent->body.vy += 0.1;
            BodyLimitSpeed(&ent->body, 2.0);
            if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
                ent->body.vx *= 0.6;
            if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
                ent->body.vy *= 0.6;

        } break;
    }
}


State *StateLoadFromFile(const char *fname)
{
    State *state = malloc(sizeof(State));
    assert(state != NULL);
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

            if (cell=='@')
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

const Entity *StateGetPlayer(const State *state){
    const Entity *ent = NULL;
    for (int i = 0; i < state->entsN; i++)
    {
        if (state->ents[i].type == TYPE_PLAYER)
            ent = &state->ents[i];
    }
    return ent;
}

void StateUpdate(State *state)
{
    for (int i = 0; i < state->entsN; i++)
    {
        Entity *ent = &state->ents[i];
        int colliding = UpdateBody(state->level, &ent->body);
        StateUpdateEntity(state, ent, colliding);
    }
}
