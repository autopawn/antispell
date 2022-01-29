#include "custom_raylib.h"

#include <math.h>
#include <stdio.h>

static int modulo(int a, int b) {
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

void DrawTextureTiledFill(Texture2D texture, Rectangle source, Vector2 origin, Color tint)
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    int timesX = 1+(int)ceilf(screenW / source.width);
    int timesY = 1+(int)ceilf(screenH / source.height);

    int startX = modulo(origin.x, source.width);
    int startY = modulo(origin.y, source.height);

    startX -= source.width;
    startY -= source.height;

    for (int y = 0; y < timesY; y++)
    {
        for (int x = 0; x < timesX; x++)
        {
            DrawTexture(texture, startX + x*source.width, startY + y*source.height, tint);
        }
    }
}
