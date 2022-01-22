/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
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

#include <stdio.h>
#include <string.h>

#include "screens.h"
#include "state.h"
#include "draw.h"


const float HLEV_ZOOM_MULT = 1.04;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

Vector2 camCenter;
static State *state;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    state = StateLoadFromFile("resources/levels/00.txt");
    LevelPrint(state->level);

    framesCounter = 0;
    finishScreen = 0;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // Update camera
    for (int i = 0; i < state->entsN; i++)
    {
        Entity *ent = &state->ents[i];
        if (ent->type == TYPE_PLAYER){
            if (framesCounter == 0)
            {
                camCenter = (Vector2){ent->body.x, ent->body.y};
            }
            else
            {
                camCenter.x = 0.8*camCenter.x + 0.2*ent->body.x;
                camCenter.y = 0.8*camCenter.y + 0.2*ent->body.y;
            }
        }
    }


    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

    framesCounter++;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, BLACK);

    Camera2D cam;
    cam.target = camCenter;
    cam.offset = (Vector2){screenW/2, screenH/2};
    cam.rotation = 0;

    cam.zoom = 1.0 / HLEV_ZOOM_MULT;
    BeginMode2D(cam);
        DrawState(state, 0);
    EndMode2D();

    cam.zoom = 1.0;
    BeginMode2D(cam);
        DrawState(state, 1);
    EndMode2D();

    cam.zoom = HLEV_ZOOM_MULT;
    BeginMode2D(cam);
        DrawState(state, 2);
    EndMode2D();

    DrawTextEx(font, "GAMEPLAY SCREEN", (Vector2){ 20, 10 }, font.baseSize*3, 4, MAROON);
    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    StateFree(state);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
