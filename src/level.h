#ifndef LEVEL_H
#define LEVEL_H

#define MAX_LEVEL_CELLS_Y 400
#define MAX_LEVEL_CELLS_X 300

typedef struct {
    int sizeY, sizeX;
    char cells[MAX_LEVEL_CELLS_Y+2][MAX_LEVEL_CELLS_X+3];
} Level;


Level *LevelLoadFromFile(const char *fname);

Level *LevelCopy(Level *level);

void LevelFree(Level *level);

void LevelPrint(const Level *level);

#endif
