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

static Vector2 camCenter;
static State *state;
static int wandAbsorving;

static Texture2D floorTexture;
static Sound timeSpeedSfx[2];

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    state = StateLoadFromFile("resources/levels/00.txt");
    LevelPrint(state->level);

    floorTexture = LoadTexture("resources/fabric61.png");
    DrawLoadResources();

    timeSpeedSfx[0] = LoadSound("resources/sfx/time_slowdown.wav");
    timeSpeedSfx[1] = LoadSound("resources/sfx/time_speedup.wav");

    framesCounter = 0;
    finishScreen = 0;
    wandAbsorving = 0;

    const Entity *player = StateGetPlayer(state);
    if (player)
        camCenter = (Vector2){player->body.x, player->body.y};
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (state->wand.absorbingTime > 0)
    {
        if (!wandAbsorving)
            PlaySound(timeSpeedSfx[0]);
        StateUpdate(state, 1);
        wandAbsorving = 1;
    }
    else
    {
        if (wandAbsorving)
            PlaySound(timeSpeedSfx[1]);
        for (int i = 0; i < 4; i++) StateUpdate(state, i == 0);
        wandAbsorving = 0;
    }

    // Update camera
    const Entity *player = StateGetPlayer(state);
    if (player)
    {
        camCenter.x = 0.8*camCenter.x + 0.2*player->lookX;
        camCenter.y = 0.8*camCenter.y + 0.2*player->lookY;
    }


    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
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

    DrawRectangle(0, 0, screenW, screenH, DARKGRAY);

    Camera2D cam;
    cam.target = camCenter;
    cam.offset = (Vector2){screenW/2, screenH/2};
    cam.rotation = 0;

    cam.zoom = 1.0 / HLEV_ZOOM_MULT;
    BeginMode2D(cam);
        DrawState(state, LAYER0_RUG);
    EndMode2D();

    DrawTextureTiled(floorTexture,
        (Rectangle){cam.target.x*cam.zoom, cam.target.y*cam.zoom, floorTexture.width, floorTexture.height},
        (Rectangle){0, 0, screenW, screenH}, (Vector2){0,0}, 0, 1, WHITE);

    cam.zoom = 1.0 / HLEV_ZOOM_MULT;
    BeginMode2D(cam);
        DrawState(state, LAYER1_FLOOR);
    EndMode2D();

    cam.zoom = 1.0;
    BeginMode2D(cam);
        DrawState(state, LAYER2_ENTS);
    EndMode2D();

    cam.zoom = HLEV_ZOOM_MULT;
    BeginMode2D(cam);
        DrawState(state, LAYER3_WALLS);
    EndMode2D();

    DrawGUI(state);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(floorTexture);
    UnloadSound(timeSpeedSfx[0]);
    UnloadSound(timeSpeedSfx[1]);
    DrawUnloadResources();
    StateFree(state);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
