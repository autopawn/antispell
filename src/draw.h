#ifndef DRAW_H
#define DRAW_H

#include "state.h"

void DrawLoadResources();
void DrawUnloadResources();

void DrawState(State *state, int hlev);
void DrawGUI(State *state);


#endif
