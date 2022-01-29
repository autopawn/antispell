#include "state.h"

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "spell_catalog.h"

static const float WAND_RADIOUS = 30;
static const int WAND_ABSORV_TIME = 20;

static const float ENEMY_VISION_RANGE = 300;

static const float PLAYER_SPEED = 2.0;
static const float SPELL_SPEED = 4.0;

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
    if (state->queuedEntsN == state->queuedEntsCapacity)
    {
        state->queuedEntsCapacity *= 2;
        state->queuedEnts = realloc(state->queuedEnts, sizeof(Entity) * state->queuedEntsCapacity);
        assert(state->queuedEnts != NULL);
    }

    Entity *ent = &state->queuedEnts[state->queuedEntsN];
    state->queuedEntsN++;
    memset(ent, 0, sizeof(Entity));
    ent->type = type;
    ent->initialBody = body;
    ent->body = body;
    ent->body.rad = (body.rad == 0)? 0.25*TS : body.rad;
    ent->powerChar = powerChar;
    if (ent->type == TYPE_MAGE)
        ent->coins = 10000;
    if (ent->type == TYPE_CHOMP)
        ent->coins =  2000;

    return ent;
}

static void StateUnqueueNewEntities(State *state)
{
    for (int i = 0; i < state->queuedEntsN; i++)
    {
        // Extend vector capacity
        if (state->entsN == state->entsCapacity)
        {
            state->entsCapacity *= 2;
            state->ents = realloc(state->ents, sizeof(Entity) * state->entsCapacity);
            assert(state->ents != NULL);
        }

        state->ents[state->entsN] = state->queuedEnts[i];
        state->entsN++;
    }
    state->queuedEntsN = 0;
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

    state->queuedEntsCapacity = 4;
    state->queuedEnts = malloc(sizeof(Entity) * state->queuedEntsCapacity);
    assert(state->queuedEnts != NULL);
    state->queuedEntsN = 0;

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
            if (cell=='R') StateAddEntity(state, TYPE_CHOMP, 'R', body);
            if (cell=='E') StateAddEntity(state, TYPE_MAGE, 'E', body);
            if (cell=='F') StateAddEntity(state, TYPE_MAGE, 'F', body);
        }
    }

    StateUnqueueNewEntities(state);

    #ifdef SPELL
        strcpy(state->wand.text, SPELL);
    #endif
    state->wand.spell = GetSpell(state->wand.text);

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

    state2->queuedEntsN = state1->queuedEntsN;
    state2->queuedEnts = malloc(sizeof(Entity) * state1->queuedEntsN);
    assert(state2->queuedEnts != NULL);
    memcpy(state2->queuedEnts, state1->queuedEnts, sizeof(Entity) * state1->queuedEntsN);

    return state2;
}

void StateFree(State *state)
{
    free(state->ents);
    free(state->queuedEnts);
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

static void AddExplosion(Entity *ent, int nParticles, Color col, char character)
{
    if (nParticles > MAX_ENTITY_PARTICLES)
        nParticles = MAX_ENTITY_PARTICLES;
    for (int i = 0; i < nParticles; i++)
    {
        const float MAX_PARTICLE_SPD = 1.0;
        Particle part = {0};
        part.body.vx = (rand()%101 - 50)/50.0 * MAX_PARTICLE_SPD;
        part.body.vy = (rand()%101 - 50)/50.0 * MAX_PARTICLE_SPD;
        part.body.x = part.body.vx*0.7*ent->body.rad/MAX_PARTICLE_SPD;
        part.body.y = part.body.vy*0.7*ent->body.rad/MAX_PARTICLE_SPD;
        part.body.rad = 9;
        part.character = character;
        part.color = col;
        part.above = 1;
        part.lifeTime = 40 + rand()%40;
        AddParticle(ent->particles, &ent->particlesN, MAX_ENTITY_PARTICLES, part);
    }
}

static void AddStateExplosion(State *state, int nParticles, Color col, char character,
        float x, float y, float rad)
{
    if (nParticles > MAX_STATE_PARTICLES)
        nParticles = MAX_STATE_PARTICLES;
    for (int i = 0; i < nParticles; i++)
    {
        const float MAX_PARTICLE_SPD = 1.0;
        Particle part = {0};
        part.body.x = x;
        part.body.y = y;
        part.body.vx = (rand()%101 - 50)/50.0 * MAX_PARTICLE_SPD;
        part.body.vy = (rand()%101 - 50)/50.0 * MAX_PARTICLE_SPD;
        BodyLimitSpeed(&part.body, MAX_PARTICLE_SPD);
        part.body.x += part.body.vx*0.7*rad/MAX_PARTICLE_SPD;
        part.body.y += part.body.vy*0.7*rad/MAX_PARTICLE_SPD;
        part.body.rad = 6;
        part.character = character;
        part.color = col;
        part.above = 1;
        part.lifeTime = 40 + rand()%40;
        AddParticle(state->particles, &state->particlesN, MAX_STATE_PARTICLES, part);
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
            UpdateBody(NULL, &parts[k].body, 1.0);
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
                part.color = ent->spell.color2;
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
    ent->cooldown -= (ent->status != STATUS_ASTONISHED && ent->status != STATUS_LAUGH && ent->status != STATUS_COOL);
    ent->cooldown -= (ent->status == STATUS_COOL)*(rand()%4==0);
    ent->cooldown -= 3*(ent->status == STATUS_ANGRY);
    ent->cooldown -= 2*(ent->status == STATUS_CEO);
    ent->cooldown -= 3*(ent->status == STATUS_CRAZY);

    { // Status particles
        if (ent->status == STATUS_ONFIRE)
            AddExplosion(ent, 1, RED, '^');
    }

    { // Status control
        if (ent->status == STATUS_ONFIRE)
        {
            if (ent->statusTime >= 100) ent->terminate = 1;
        }
        else if (ent->status == STATUS_FROZEN || ent->status == STATUS_CEO)
        {
            if (ent->statusTime >= 1000){
                ent->status = STATUS_NORMAL;
                ent->statusTime = 0;
            }
        }
        else if (ent->status != STATUS_NORMAL && ent->status != STATUS_FREE
            && ent->status != STATUS_LOOSE && ent->status != STATUS_COOL)
        {
            if (ent->statusTime >= 600)
            {
                ent->status = STATUS_NORMAL;
                ent->statusTime = 0;
            }
        }
        ent->statusTime++;

        if (ent->status == STATUS_FROZEN || ent->status == STATUS_ASTONISHED)
        {
            ent->body.vx = 0;
            ent->body.vy = 0;
            return;
        }
        if (ent->status == STATUS_CRAZY || (ent->status == STATUS_LOOSE && ent->timeAlive%150 == 0))
        {
            ent->lookX = ent->body.x + rand()%301 - 150;
            ent->lookY = ent->body.y + rand()%301 - 150;
        }
    }

    if (ent->status == STATUS_COOL)
    {
        ent->body.x += rand()%3 - 1;
        ent->body.y += rand()%3 - 1;
    }

    // Current cell
    int cellX = ent->body.x/TS;
    int cellY = ent->body.y/TS;
    char floor = LevelGetAt(state->level, cellY, cellX);

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
            BodyLimitSpeed(&ent->body, PLAYER_SPEED);
            if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))
                ent->body.vx *= 0.6;
            if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
                ent->body.vy *= 0.6;

            // Look at the mouse
            float lookDeltaX = GetMouseX() - GetScreenWidth()/2.0;
            float lookDeltaY = GetMouseY() - GetScreenHeight()/2.0;
            ent->lookX = ent->body.x + lookDeltaX/2.0;
            ent->lookY = ent->body.y + lookDeltaY/2.0;

            if (process_pressed_keys && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && state->wand.text[0] != '\0')
            {
                state->wand.text[strlen(state->wand.text) - 1] = '\0';
                state->wand.spell = GetSpell(state->wand.text);
                state->wand.signal = WANDSIGNAL_BACKSPACE;
                state->wand.signalIntensity = 1.0;
            }
            // Attack update
            int spellValid = state->wand.spell.type != SPELLTYPE_NONE;
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


                if (BodySetSpeed(&body, SPELL_SPEED) == 1)
                {
                    Entity *attack = StateAddEntity(state, TYPE_SPELL, 0, body);
                    attack->spell = state->wand.spell;
                    if (attack->spell.type == SPELLTYPE_LOL)
                        attack->body.rad *= 2;

                    state->wand.signal = WANDSIGNAL_SPELL;
                    state->wand.signalIntensity = 1.0;

                    assert(ent != NULL);
                    ent->attackCalled = 0;
                    ent->cooldown = 30;
                }
            }

            if (process_pressed_keys && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                ent->attackCalled = 1;

            for(int i = 0; i < state->entsN; i++)
            {
                Entity *other = &state->ents[i];
                int colli = 0;
                colli = colli || other->type == TYPE_PROJECTILE;
                colli = colli || other->type == TYPE_CHOMP;
                colli = colli || other->type == TYPE_MAGE;
                if (colli && BodyDistance(ent->body, other->body) < -ent->body.rad/3)
                {
                    if (other->powerChar == '$')
                    {
                        ent->coins += 100;
                        other->terminate = 1;
                    }
                    else
                    {
                        ent->terminate = 1;
                    }
                }
            }
            break;
        }

        case TYPE_MAGE:
        {
            const Entity *player = StateGetPlayer(state);
            if (player &&
                    (BodyDistance(ent->body, player->body) < ENEMY_VISION_RANGE || ent->status == STATUS_ANGRY) &&
                    LineOfSight(state->level, ent->body, player->body))
            {
                if (ent->status != STATUS_CRAZY && ent->status != STATUS_LOOSE)
                {
                    ent->lookX = player->body.x;
                    ent->lookY = player->body.y;
                }
                if (ent->cooldown < 0)
                { // Shoot

                    if (ent->powerChar == 'F')
                    {
                        ent->cooldown = 40;
                        ent->lookX += rand()%201 - 100;
                        ent->lookY += rand()%201 - 100;
                    }
                    else
                    {
                        ent->cooldown = 200;
                    }

                    Body body = {
                        .x=ent->body.x,
                        .y=ent->body.y,
                        .vx = ent->lookX - ent->body.x,
                        .vy = ent->lookY - ent->body.y,
                        .rad = 0.85*ent->body.rad,
                    };
                    BodyLimitSpeed(&body, 2.0);
                    if (ent->status == STATUS_COOL)
                        BodyLimitSpeed(&body, 0.5);
                    char powerChar = ent->powerChar;
                    if (ent->status == STATUS_CEO) powerChar = '$';
                    StateAddEntity(state, TYPE_PROJECTILE, powerChar, body);
                }
            }
            if (ent->status == STATUS_FREE || ent->status == STATUS_LOOSE)
                BodyMoveTowards(&ent->body, ent->lookX, ent->lookY, 0.7);
            break;
        }

        case TYPE_CHOMP:
        {
            const Entity *player = StateGetPlayer(state);
            if (player && LineOfSight(state->level, ent->body, player->body)
                    && ent->status != STATUS_CRAZY && ent->status != STATUS_LOOSE)
            {
                ent->lookX = player->body.x;
                ent->lookY = player->body.y;
            }
            BodyMoveTowards(&ent->body, ent->lookX, ent->lookY, (ent->status != STATUS_NORMAL)? 0.4 : 4);

            if (ent->status != STATUS_FREE && ent->status != STATUS_LOOSE)
            {
                float deltaXini = ent->body.x - ent->initialBody.x;
                float deltaYini = ent->body.y - ent->initialBody.y;
                LimitVector(&deltaXini, &deltaYini, (ent->powerChar=='R')? 160 : 60);
                ent->body.x = ent->initialBody.x + deltaXini;
                ent->body.y = ent->initialBody.y + deltaYini;
            }
            else
            {
                ent->initialBody.x = ent->body.x;
                ent->initialBody.y = ent->body.y;
            }
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
            // Affect other entities
            for(int i = 0; i < state->entsN; i++)
            {
                Entity *other = &state->ents[i];
                int colli = 1;
                colli = colli && other->type != TYPE_PLAYER;
                colli = colli && other->type != TYPE_SPELL;
                colli = colli && other->type != TYPE_PROJECTILE;
                if (colli && BodyDistance(ent->body, other->body) <= 0)
                {
                    switch (ent->spell.type)
                    {
                        case SPELLTYPE_ICE:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_FROZEN;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_FIRE:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_ONFIRE;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_FEE:
                        case SPELLTYPE_IRE:
                        case SPELLTYPE_REIF:
                        {
                            ent->terminate = 1;
                            if (other->type == TYPE_MAGE || other->type == TYPE_CHOMP)
                            {
                                other->status = STATUS_ANGRY;
                                other->statusTime = 0;
                                int coins_lost = 0;
                                if (ent->spell.type == SPELLTYPE_FEE)
                                    coins_lost = (other->coins + 9)/10;
                                if (ent->spell.type == SPELLTYPE_REIF)
                                    coins_lost = 1000;
                                if (other->coins < coins_lost)
                                    coins_lost = other->coins;

                                Entity *player = StateGetPlayer(state);
                                other->coins -= coins_lost;
                                if (player)
                                    player->coins += coins_lost;
                                AddExplosion(other, (coins_lost+99)/100, YELLOW, '$');
                            }
                            break;
                        }
                        case SPELLTYPE_RICE:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_YUMMY;
                            other->statusTime = 0;
                            other->body.rad += 4;
                            break;
                        }
                        case SPELLTYPE_LESS:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_ASTONISHED;
                            other->statusTime = 0;
                            other->body.rad *= 0.9;
                            if (other->body.rad < 3) other->terminate = 1;
                            break;
                        }
                        case SPELLTYPE_SELL:
                        {
                            ent->terminate = 1;
                            other->terminate = 1;
                            Entity *player = StateGetPlayer(state);
                            if (player)
                                player->coins += 100*other->body.rad;
                            AddStateExplosion(state, 100, YELLOW, '$', other->body.x, other->body.y, other->body.rad);
                            break;
                        }
                        case SPELLTYPE_CELL:
                        {
                            ent->terminate = 1;
                            other->body.rad *= 0.8;
                            float angle = 2*M_PI*rand()/(float)RAND_MAX;
                            Body body2 = other->body;
                            body2.x += 2*other->body.rad*cosf(angle);
                            body2.y += 2*other->body.rad*sinf(angle);
                            other->body.x -= 2*other->body.rad*cosf(angle);
                            other->body.y -= 2*other->body.rad*sinf(angle);
                            other->initialBody = other->body;
                            if (other->body.rad < 3)
                                other->terminate = 1;
                            else
                                StateAddEntity(state, other->type, other->powerChar, body2);
                            break;
                        }
                        case SPELLTYPE_FREE:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_FREE;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_LOL:
                        {
                            other->status = STATUS_LAUGH;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_CEO:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_CEO;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_LOCO:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_CRAZY;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_LOOSE:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_LOOSE;
                            other->statusTime = 0;
                            break;
                        }
                        case SPELLTYPE_LOSS:
                        {
                            ent->terminate = 1;
                            other->terminate = 1;
                            AddStateExplosion(state, 30, DARKGRAY, '?',
                                    other->body.x, other->body.y, other->body.rad);
                            int otherCellX = other->body.x/TS;
                            int otherCellY = other->body.y/TS;
                            char otherFloor = LevelGetAt(state->level, otherCellY, otherCellX);
                            if (!LevelCellIsSolid(otherFloor) && otherFloor != '$')
                                state->level->cells[otherCellY][otherCellX] = 'o';
                            break;
                        }
                        case SPELLTYPE_COOL:
                        {
                            ent->terminate = 1;
                            other->status = STATUS_COOL;
                            other->statusTime = 0;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }

            if (ent->spell.type == SPELLTYPE_SEE && ent->timeAlive > 40)
                ent->terminate = 1;

            if (LevelCellIsSolid(floor))
                ent->terminate = 1;
            // Froze lava floor
            if ((ent->spell.type == SPELLTYPE_ICE || ent->spell.type == SPELLTYPE_COOL) && floor == 'l')
            {
                state->level->cells[cellY][cellX] = 'a'; // ashes
                ent->terminate = 1;
            }
            // Burn wood or reef
            if (ent->spell.type == SPELLTYPE_FIRE && (floor == 'w' || floor == 'r'))
            {
                state->level->cells[cellY][cellX] = 'a'; // ashes
                ent->terminate = 1;
            }
            // Generate reef
            if (ent->spell.type == SPELLTYPE_REEF && floor == ' ')
            {
                state->level->cells[cellY][cellX] = 'r'; // reef
            }

            // Distract other entities
            if ((ent->spell.type == SPELLTYPE_REFER || ent->spell.type == SPELLTYPE_SEE) && ent->terminate == 1)
            {
                for(int i = 0; i < state->entsN; i++)
                {
                    Entity *other = &state->ents[i];
                    if (other->type == TYPE_MAGE || other->type == TYPE_CHOMP)
                    {
                        if (other->status <= STATUS_ASTONISHED)
                        {
                            other->status = STATUS_ASTONISHED;
                            other->statusTime = 0;
                            other->lookX = ent->body.x;
                            other->lookY = ent->body.y;
                        }
                    }
                }
            }
            // TP the player
            if (ent->spell.type == SPELLTYPE_ELSE)
            {
                if (floor == ' ')
                {
                    ent->initialBody.x = (cellX + 0.5)*TS;
                    ent->initialBody.y = (cellY + 0.5)*TS;
                }

                if (ent->terminate)
                {
                    Entity *player = StateGetPlayer(state);
                    if (player)
                    {
                        player->body.x = ent->initialBody.x;
                        player->body.y = ent->initialBody.y;
                    }
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

            if (ent->type == TYPE_PLAYER)
            {
                AddStateExplosion(state, 100, GRAY, '@', ent->body.x, ent->body.y, ent->body.rad);
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

        int spellLen = strlen(state->wand.text);
        if (state->wand.absorbingTime >= WAND_ABSORV_TIME && spellLen < MAX_SPELL_LENGHT)
        {
            state->wand.text[spellLen] = state->wand.absorbingChar;
            state->wand.text[spellLen + 1] = '\0';
            state->wand.spell = GetSpell(state->wand.text);
            state->wand.absorbingChar = 0;
            state->wand.absorbingTime = 0;
            state->wand.signal = WANDSIGNAL_ABSORBED;
            state->wand.signalIntensity = 1.0;
            spellLen++;
            if (state->wand.spell.type != SPELLTYPE_NONE) PlaySoundMulti(wandBellSfx);
        }

        Entity *player = StateGetPlayer(state);
        int absorving = 0;
        if (player)
        {
            for (int i = 0; i < state->entsN; i++)
            {
                Entity *ent = &state->ents[i];
                if (ent->powerChar == 0 || ent->powerChar == '$')
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
            if (spellLen == MAX_SPELL_LENGHT || state->wand.spell.type != SPELLTYPE_NONE)
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
        // Environment modifiers
        float slowFactor = 1.0;
        // Current cell
        int cellX = ent->body.x/TS;
        int cellY = ent->body.y/TS;
        char floor = LevelGetAt(state-> level, cellY, cellX);
        if (floor == 'r')
        {
            slowFactor = 0.08;
            if (ent->type == TYPE_PLAYER)     slowFactor = 0.8;
            if (ent->type == TYPE_SPELL)      slowFactor = 0.8;
            if (ent->type == TYPE_PROJECTILE) slowFactor = 0.3;
        }
        else if (floor == 'l' && ent->status < STATUS_ONFIRE)
        {
            ent->status = STATUS_ONFIRE;
            ent->statusTime = 0;
        }
        // Update physics
        int colliding = UpdateBody((ent->type == TYPE_SPELL)? NULL : state->level,
                &ent->body, slowFactor);
        // Update entity
        StateUpdateEntity(state, ent, colliding, process_pressed_keys);
    }

    // Udpate state result
    state->resultTime++;
    if (state->result == STATERESULT_CONTINUE)
    {
        Entity *player = StateGetPlayer(state);
        if (!player)
        {
            state->result = STATERESULT_RETRY;
            state->resultTime = 0;
        }
        else
        {
            int playerCellX = player->body.x/TS;
            int playerCellY = player->body.y/TS;
            if (LevelGetAt(state->level, playerCellY, playerCellX) == '$')
            {
                state->result = STATERESULT_NEXTLEVEL;
                state->resultTime = 0;
            }
        }
    }

    // Move queued entities to real entity array
    StateUnqueueNewEntities(state);

    // Update frame counter
    state->frame++;
}
