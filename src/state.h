#ifndef STATE_H
#define STATE_H

#include "level.h"
#include "physics.h"

#define MAX_SPELL_LENGHT 5

typedef enum {
    TYPE_PLAYER = '@',
    TYPE_PLANT_I = 'I',
    TYPE_PLANT_E = 'E',
    TYPE_MAGE_C = 'C',
    TYPE_PROJECTILE,
} EntityType;

typedef struct {
    Body body;
    EntityType type;
    // Char that can be absorved by the wand
    char powerChar;
    // Position were it is looking
    float lookX, lookY;

    int cooldown;
} Entity;

typedef enum
{
    WANDSIGNAL_ABSORB = 0,
    WANDSIGNAL_ABSORBED = 1,
    WANDSIGNAL_FULL = 2,
    WANDSIGNAL_BACKSPACE = 3,
} WandSignal;

typedef struct {
    Level *level;

    int entsCapacity;
    int entsN;
    Entity *ents;

    struct {
        // Words forming in the wand
        char spell[MAX_SPELL_LENGHT + 1];
        char absorbingChar;
        int absorbingTime;
        WandSignal signal;
        float signalIntensity;
    } wand;
} State;


State *StateLoadFromFile(const char *fname);

State *StateCopy(const State *state);

void StateFree(State *state);

void StateUpdate(State *state, int process_pressed_keys);

Entity *StateGetPlayer(const State *state);



#endif

