#ifndef DRAW_H
#define DRAW_H

#include "state.h"

typedef enum {
    LAYER0_RUG   = 0,
    LAYER1_FLOOR = 1,
    LAYER2_ENTS  = 2,
    LAYER3_WALLS = 3,
} DrawLayer;

void DrawLoadResources();
void DrawUnloadResources();

void DrawState(State *state, DrawLayer layer);
void DrawGUI(State *state);


#endif
