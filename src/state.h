#ifndef STATE_H
#define STATE_H

#include "level.h"
#include "physics.h"

#define MAX_SPELL_LENGHT 5

typedef enum {
    TYPE_PLAYER = '@',
} EntityType;

typedef struct {
    Body body;
    EntityType type;
} Entity;

typedef struct {
    Level *level;

    int entsCapacity;
    int entsN;
    Entity *ents;

    struct {
        // Words forming in the wand
        char spell[MAX_SPELL_LENGHT + 1];
        char absorvingChar;
        int absorvingTime;
    } wand;
} State;


State *StateLoadFromFile(const char *fname);

State *StateCopy(const State *state);

void StateFree(State *state);

void StateUpdate(State *state);

const Entity *StateGetPlayer(const State *state);



#endif

