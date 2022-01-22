#include "state.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const int TS = LEVEL_TILE_SIZE;

State *StateLoadFromFile(const char *fname)
{
    State *state = malloc(sizeof(State));
    assert(state != NULL);
    state->level = LevelLoadFromFile(fname);

    int entsSize = 4;
    state->ents = malloc(sizeof(Entity) * entsSize);
    assert(state->ents != NULL);
    state->entsN = 0;

    // Add entities from the map
    for (int y = 0; y < state->level->sizeY; y++)
    {
        for (int x = 0; x < state->level->sizeX; x++)
        {
            char cell = state->level->cells[y][x];
            Entity *ent = &state->ents[state->entsN];

            switch (cell)
            {
                case '@':
                    state->entsN++;
                    ent->type = TYPE_PLAYER;
                    ent->body = (Body){(x + 0.5)*TS, (y + 0.5)*TS, 0, 0, 0.25*TS};
                break;
            }


            if (state->entsN == entsSize)
            {
                entsSize *= 2;
                state->ents = realloc(state->ents, sizeof(Entity) * entsSize);
                assert(state->ents != NULL);
            }
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

void StateUpdate(const State *state)
{

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
