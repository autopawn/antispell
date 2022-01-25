#include "state.h"

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "spell_catalog.h"

static const float WAND_RADIOUS = 30;
static const int WAND_ABSORV_TIME = 25;

static const float ENEMY_VISION_RANGE = 300;

static const int TS = LEVEL_TILE_SIZE;

static Sound wandBellSfx;

void StateLoadResources(){
    wandBellSfx = LoadSound("resources/sfx/wand_bell.wav");
}

void StateUnloadResources()
{
    UnloadSound(wandBellSfx);
}

static Entity *StateAddEntity(State *state, EntityType type, char powerChar, Body body)
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
    ent->initialBody = body;
    ent->body = body;
    ent->body.rad = (body.rad == 0)? 0.25*TS : body.rad;
    ent->powerChar = powerChar;

    switch (ent->type)
    {
        default:
        {
            break;
        }
    }
    return ent;
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

            if (cell=='@') StateAddEntity(state, TYPE_PLAYER, 0, body);
            if (cell=='I') StateAddEntity(state, TYPE_FLOWER, 'I', body);
            if (cell=='C') StateAddEntity(state, TYPE_CHOMP, 'C', body);
            if (cell=='E') StateAddEntity(state, TYPE_MAGE, 'E', body);
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

static Particle *AddParticle(Particle *parts, int *partsN, int partsMax, Particle particle)
{
    if ((*partsN) == partsMax)
    {
        int idx = rand()%partsMax;
        parts[idx] = particle;
        return &parts[idx];
    }
    else
    {
        parts[*partsN] = particle;
        *partsN += 1;
        return &parts[(*partsN) - 1];
    }
}

static void UpdateParticleList(Particle *parts, int *partsN)
{
    int particlesN2 = 0;
    for (int k = 0; k < *partsN; k++)
    {
        if (parts[k].lifeTime > 0)
        {
            parts[k].lifeTime--;
            UpdateBody(NULL, &parts[k].body);
            parts[particlesN2++] = parts[k];
        }
    }
    *partsN = particlesN2;
}

static void StateUpdateEntity(State *state, Entity *ent, int colliding, int process_pressed_keys)
{
    // Emit powerChar particles
    if (ent->type == TYPE_SPELL)
    {
        const int PARTICLE_FREQ = 4;

        if (ent->timeAlive%PARTICLE_FREQ == 0)
        {
            int p = (ent->timeAlive/PARTICLE_FREQ)%(strlen(ent->spell.name) + 1);
            if (p < strlen(ent->spell.name))
            {
                Particle part = {0};
                part.body.x = ent->body.x;
                part.body.y = ent->body.y;
                part.body.rad = 4;
                part.lifeTime = 40;
                part.character = ent->spell.name[p];
                part.color = ent->spell.color;
                AddParticle(state->particles, &state->particlesN, MAX_STATE_PARTICLES, part);
            }
        }
    } else {
        const float PARTICLE_SPD = 0.4;
        const int PARTICLE_FREQ = 16;

        if (ent->timeAlive%PARTICLE_FREQ == 0 && ent->powerChar != 0)
        {
            float angle = 0.5*M_PI + ((137.508/360)*2*M_PI)*(ent->timeAlive/PARTICLE_FREQ);  // Golden angle
            Particle part = {0};
            part.body.vx = PARTICLE_SPD*cos(angle);
            part.body.vy = PARTICLE_SPD*sin(angle);
            part.body.rad = 4;
            part.lifeTime = 80;
            part.character = ent->powerChar;
            part.color = GetPowerCharColor(ent->powerChar);
            AddParticle(ent->particles, &ent->particlesN, MAX_ENTITY_PARTICLES, part);
        }
    }
    // Timers
    ent->timeAlive++;
    ent->cooldown--;

    { // Status control
        if (ent->status == STATUS_FROZEN && ent->statusTime >= 800)
        {
            ent->status = STATUS_NORMAL;
            ent->statusTime = 0;
        }
        ent->statusTime++;

        if (ent->status == STATUS_FROZEN)
        {
            ent->body.vx = 0;
            ent->body.vy = 0;
            return;
        }
    }


    // Current cell
    int cellX = ent->body.x/TS;
    int cellY = ent->body.y/TS;
    char floor = LevelGetAt(state-> level, cellY, cellX);

    // Entity intelligence
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

            if (process_pressed_keys && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && state->wand.spell[0] != '\0')
            {
                state->wand.spell[strlen(state->wand.spell) - 1] = '\0';
                state->wand.signal = WANDSIGNAL_BACKSPACE;
                state->wand.signalIntensity = 1.0;
            }
            // Attack update
            Spell spell = GetSpell(state->wand.spell);
            int spellValid = spell.type != SPELLTYPE_NONE;
            if (!spellValid)
                ent->attackCalled = 0;

            if (ent->attackCalled && ent->cooldown < 0)
            {
                // Cast spell
                Body body = {
                    .x=ent->body.x,
                    .y=ent->body.y,
                    .vx = ent->lookX - ent->body.x,
                    .vy = ent->lookY - ent->body.y,
                    .rad = 0.3*TS,
                };
                if (BodySetSpeed(&body, 4.0) == 1)
                {
                    Entity *attack = StateAddEntity(state, TYPE_SPELL, 0, body);
                    attack->spell = spell;

                    state->wand.signal = WANDSIGNAL_SPELL;
                    state->wand.signalIntensity = 1.0;

                    ent->attackCalled = 0;
                    ent->cooldown = 30;
                }
            }

            if (process_pressed_keys && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                ent->attackCalled = 1;
            break;
        }

        case TYPE_MAGE:
        {
            const Entity *player = StateGetPlayer(state);
            if (BodyDistance(ent->body, player->body) < ENEMY_VISION_RANGE &&
                        LineOfSight(state->level, ent->body, player->body))
            {
                ent->lookX = player->body.x;
                ent->lookY = player->body.y;
                if (ent->cooldown < 0)
                {
                    ent->cooldown = 200;

                    Body body = {
                        .x=ent->body.x,
                        .y=ent->body.y,
                        .vx = player->body.x - ent->body.x,
                        .vy = player->body.y - ent->body.y,
                    };
                    BodyLimitSpeed(&body, 2.0);
                    StateAddEntity(state, TYPE_PROJECTILE, ent->powerChar, body);
                }
            }
            break;
        }

        case TYPE_CHOMP:
        {
            const Entity *player = StateGetPlayer(state);
            if (BodyDistance(ent->body, player->body) < ENEMY_VISION_RANGE &&
                        LineOfSight(state->level, ent->body, player->body))
            {
                ent->lookX = player->body.x;
                ent->lookY = player->body.y;
                BodyAccelTowards(&ent->body, ent->lookX, ent->lookY, 0.1, 4);
            }
            float deltaXini = ent->body.x - ent->initialBody.x;
            float deltaYini = ent->body.y - ent->initialBody.y;
            LimitVector(&deltaXini, &deltaYini, 60);
            ent->body.x = ent->initialBody.x + deltaXini;
            ent->body.y = ent->initialBody.y + deltaYini;
            break;
        }

        case TYPE_PROJECTILE:
        {
            if (colliding)
                ent->terminate = 1;
            break;
        }

        case TYPE_SPELL:
        {
            if (colliding)
                ent->terminate = 1;
            // Froze lave floor
            if (floor == 'l')
            {
                state->level->cells[cellY][cellX] = ' ';
                ent->terminate = 1;
            }

            // Froze other entities
            for(int i = 0; i < state->entsN; i++)
            {
                Entity *other = &state->ents[i];
                int colli = 1;
                colli = colli && other->type != TYPE_PLAYER;
                colli = colli && other->type != TYPE_SPELL;
                colli = colli && other->type != TYPE_PROJECTILE;
                if (colli && ent->spell.type == SPELLTYPE_ICE && BodyDistance(ent->body, other->body) <= 0)
                {
                    other->status = STATUS_FROZEN;
                    other->statusTime = 0;
                    ent->terminate = 1;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
}


void StateUpdate(State *state, int process_pressed_keys)
{
    // Destroy entities marked for termination
    int entsN2 = 0;
    for(int i = 0; i < state->entsN; i++)
    {
        Entity *ent = &state->ents[i];
        if (ent->terminate)
        {
            // Move entity particles to the state
            for(int k = 0; k < ent->particlesN; k++)
            {
                Particle particle = ent->particles[k];
                particle.body.x += ent->body.x;
                particle.body.y += ent->body.y;
                AddParticle(state->particles, &state->particlesN, MAX_STATE_PARTICLES, particle);
            }
        }
        else
        {
            state->ents[entsN2] = *ent;
            entsN2++;
        }
    }
    state->entsN = entsN2;

    { // Update particles
        UpdateParticleList(state->particles, &state->particlesN);
        for(int i = 0; i < state->entsN; i++)
            UpdateParticleList(state->ents[i].particles, &state->ents[i].particlesN);
    }

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
            Spell spell = GetSpell(state->wand.spell);
            if (spell.type != SPELLTYPE_NONE) PlaySoundMulti(wandBellSfx);
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

    // Update frame counter
    state->frame++;
}
