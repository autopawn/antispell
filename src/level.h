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
    return cell == '#' || cell == '~' || cell == 'w';
}

static inline char LevelGetAt(const Level *level, int y, int x)
{
    if (x < 0) return '~';
    if (y < 0) return '~';
    if (x >= level->sizeX) return '~';
    if (y >= level->sizeY) return '~';
    return level->cells[y][x];
}


Level *LevelLoadFromFile(const char *fname);

Level *LevelCopy(Level *level);

void LevelFree(Level *level);

void LevelPrint(const Level *level);

#endif
