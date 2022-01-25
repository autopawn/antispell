#ifndef STATE_H
#define STATE_H

#include "level.h"
#include "spell_catalog.h"
#include "physics.h"

#define MAX_SPELL_LENGHT 5

typedef enum {
    TYPE_PLAYER,
    TYPE_FLOWER,
    TYPE_CHOMP,
    TYPE_MAGE,
    TYPE_PROJECTILE,
    TYPE_SPELL,
} EntityType;

typedef enum {
    STATUS_NORMAL,
    STATUS_FROZEN,
} EntityStatus;

typedef struct {
    Body body;
    EntityType type;
    int terminate;
    // Char that can be absorved by the wand
    char powerChar;
    // Spell attributes for TYPE_SPELL entities
    Spell spell;
    // Position were it is looking
    float lookX, lookY;

    EntityStatus status;
    int statusTime;
    Body initialBody;

    int attackCalled;
    int cooldown;
} Entity;

typedef enum
{
    WANDSIGNAL_ABSORB = 0,
    WANDSIGNAL_ABSORBED = 1,
    WANDSIGNAL_FULL = 2,
    WANDSIGNAL_BACKSPACE = 3,
    WANDSIGNAL_SPELL = 4,
} WandSignal;

typedef struct {
    int frame;

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

