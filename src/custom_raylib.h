#ifndef CUSTOM_RAYLIB_H
#define CUSTOM_RAYLIB_H

#include <raylib.h>

// I had to write function since DrawTextureTiled functioned differently to expected on web
void DrawTextureTiledFill(Texture2D texture, Rectangle source, Vector2 origin, Color tint);

#endif
