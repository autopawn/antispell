#ifndef LEVEL_H
#define LEVEL_H

#define MAX_LEVEL_CELLS_Y 400
#define MAX_LEVEL_CELLS_X 300

#define LEVEL_TILE_SIZE 50

typedef struct {
    int sizeY, sizeX;
    char cells[MAX_LEVEL_CELLS_Y+2][MAX_LEVEL_CELLS_X+3];
} Level;

static inline int LevelCellIsSolid(char cell)
{
    return cell == '#' || cell == '~';
}


Level *LevelLoadFromFile(const char *fname);

Level *LevelCopy(Level *level);

void LevelFree(Level *level);

void LevelPrint(const Level *level);

#endif
