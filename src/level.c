#include "level.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "raylib.h"

Level *LevelLoadFromFile(const char *fname){
    char *text = LoadFileText(fname);
    if (!text)
    {
        fprintf(stderr, "ERROR: Could not load \"%s\".\n",fname);
        exit(1);
    }

    Level *level = malloc(sizeof(Level));
    assert(level!=NULL);
    memset(level, 0, sizeof(*level));

    int n = strlen(text);
    int y = 0, x = 0;

    for(int i = 0; i < n; i++)
    {
        char c = text[i];
        if (c=='\n')
        {
            y += 1;
            x = 0;
        }
        else
        {
            assert(x < MAX_LEVEL_CELLS_X);
            assert(y < MAX_LEVEL_CELLS_Y);
            level->cells[y + 1][x + 1] = c;
            x++;
            level->sizeY = y + 3;
            if (x + 3 > level->sizeX) level->sizeX = x + 3;
        }
    }

    //
    for (y = 0; y < level->sizeY; y++)
    {
        for (x = 0; x < level->sizeX; x++)
        {
            if (level->cells[y][x] == '\0')
                level->cells[y][x] = ' ';
        }
    }

    // Create walls around the level
    for (y = 0; y < level->sizeY; y++)
    {
        level->cells[y][0] = '#';
        level->cells[y][level->sizeX - 1] = '#';
    }
    for (x = 0; x < level->sizeX; x++)
    {
        level->cells[0][x] = '#';
        level->cells[level->sizeY - 1][x] = '#';
    }

    UnloadFileText(text);

    return level;
}

Level *LevelCopy(Level *level){
    Level *level2 = malloc(sizeof(Level));
    memcpy(level2, level, sizeof(*level2));
    return level2;
}

void LevelFree(Level *level){
    free(level);
}

void LevelPrint(const Level *level){
    printf("Level (%d x %d)\n", level->sizeY, level->sizeX);
    for (int y = 0; y < level->sizeY; y++)
    {
        printf("%s\n",level->cells[y]);
    }
}
