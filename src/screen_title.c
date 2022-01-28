/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;
static int framesCounter = 0;
static int lettersShow = 0;
static float light = 0.0;

static Texture2D backgroundTexture[2];
static Texture2D antimageTexture;

static const int PHASE1 = 10;
static const int PHASE2 = 60;
static const int PHASE3 = 130;

static const char *TITLE = "ANTISPELL";

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    backgroundTexture[0] = LoadTexture("resources/title/background0.png");
    backgroundTexture[1] = LoadTexture("resources/title/background1.png");
    antimageTexture = LoadTexture("resources/title/antimage.png");
    SetTextureFilter(antimageTexture, TEXTURE_FILTER_BILINEAR);

    framesCounter = 0;
    finishScreen = 0;
    lettersShow = 0;

    light = 0.0;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    framesCounter++;

    if (framesCounter <= PHASE1)
    {
    }
    else if (framesCounter <= PHASE2)
    {
        light = (framesCounter - PHASE1)/(float)(PHASE2 - PHASE1);
    }
    else if (framesCounter <= PHASE3)
    {

        int letters = strlen(TITLE)*(framesCounter - PHASE2)/(PHASE3 - PHASE2);
        if (letters > lettersShow)
        {
            // TODO: play sound
            lettersShow = letters;
        }
    }
    else if (IsKeyPressed(KEY_ENTER))
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
        PlaySound(fxCoin);
    }

    if (framesCounter >= PHASE2)
        light = 0.85 + 0.15 * cosf(0.03*(framesCounter - PHASE2));
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    Rectangle backgroundRect = {0, 0, backgroundTexture[0].width, backgroundTexture[0].height};
    Rectangle tgtRect = {0, 0, screenW, screenH};

    unsigned char lit = (unsigned char)(int)(255 * light * light);
    Color colorLight = (Color){lit, lit, lit, 255};

    float antimageScale = 0.4 * screenH / antimageTexture.height;

    float antimageFinalX = (screenW - antimageTexture.width*antimageScale) / 2.0;

    float mageMove = (framesCounter >= PHASE2)? 1.0 : light;
    float antimageX = antimageFinalX*(1.8 - 0.8*mageMove);
    float antimageY = (0.41 + 0.3 *(mageMove*mageMove - 1.0))*screenH;

    DrawTexturePro(backgroundTexture[0], backgroundRect, tgtRect, (Vector2){0,0}, 0, WHITE);
    DrawTextureEx(antimageTexture, (Vector2){antimageX, antimageY}, 0, antimageScale, WHITE);
    DrawTexturePro(backgroundTexture[1], backgroundRect, tgtRect, (Vector2){0,0}, 0, colorLight);
    DrawRectangle(0, 0, screenW, tgtRect.y+1, BLACK);

    for (int i = 0; i < lettersShow; i++)
    {
        const int FONT_SIZE = 64;

        float letterX = screenW*(i + 2)/(strlen(TITLE) + 3.0);
        float letterY = screenH*0.1;

        char symbol[2];
        symbol[0] = TITLE[i];
        symbol[1] = '\0';
        int letterW = MeasureText(symbol, FONT_SIZE);

        int moveX = rand()%9-4;
        int moveY = rand()%9-4;

        DrawText(symbol, letterX - letterW/2 + moveX, letterY - FONT_SIZE/2 + moveY, FONT_SIZE, WHITE);
        DrawText(symbol, letterX - letterW/2 - moveX, letterY - FONT_SIZE/2 - moveY, FONT_SIZE, WHITE);
        DrawText(symbol, letterX - letterW/2, letterY - FONT_SIZE/2, FONT_SIZE, WHITE);
    }
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    UnloadTexture(backgroundTexture[0]);
    UnloadTexture(backgroundTexture[1]);
    UnloadTexture(antimageTexture);
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}
