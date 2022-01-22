#include "state.h"

#include <stdlib.h>

State *StateLoadFromFile(const char *fname){
    State *state = malloc(sizeof(State));
    state->level = LevelLoadFromFile(fname);

    return state;
}

void StateFree(State *state){
    LevelFree(state->level);
    free(state);
}
