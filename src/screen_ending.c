/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Ending Screen Functions Definitions (Init, Update, Draw, Unload)
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

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "screens.h"
#include "custom_raylib.h"
#include "spell_catalog.h"



//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

extern int coinsCollected; // from screen_gameplay.c

static char *endingText;
static char *endingTextP;

#define ENDING_SHOWING_LINES 16
static char *showingLines[ENDING_SHOWING_LINES];
static int lineStart = 0;

static const int LINE_APPEAR_DELAY = 30;

static Texture2D floorTextureCredits;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

static inline char *StatsText()
{
    int nSpells = SPELLTYPE_LAST + 1;
    char *spellsText = malloc(sizeof(char)*100*(nSpells + 6));
    assert(spellsText != NULL);

    int txtLen = 0;

    txtLen += sprintf(&spellsText[txtLen], "=== SPELLS DISCOVERED ===\n\n");
    for (int i = 0; i < nSpells; i++)
    {
        Spell spell = GetSpellFromType(i);
        if (spell.type == SPELLTYPE_NONE) continue;
        if (spell.used == 1)
            txtLen += sprintf(&spellsText[txtLen],"%s\n",spell.name);
    }
    txtLen += sprintf(&spellsText[txtLen], "\n\n");

    txtLen += sprintf(&spellsText[txtLen], "=== SPELLS UNKNOWN ===\n\n");
    for (int i = 0; i < nSpells; i++)
    {
        Spell spell = GetSpellFromType(i);
        if (spell.type == SPELLTYPE_NONE) continue;
        if (spell.used == 0)
            txtLen += sprintf(&spellsText[txtLen],"%s\n",spell.name);
    }
    txtLen += sprintf(&spellsText[txtLen], "\n\n");

    txtLen += sprintf(&spellsText[txtLen], "=== TOTAL COINS COLLECTED ===\n\n");
    txtLen += sprintf(&spellsText[txtLen], "%d",coinsCollected);
    txtLen += sprintf(&spellsText[txtLen], "\n\n\n\n\n\n\n\n\n");

    return spellsText;
}

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    floorTextureCredits = LoadTexture("resources/fabric61.png");

    char *creditText = LoadFileText("resources/CREDITS.txt");
    char *statsText = StatsText();
    char *license = LoadFileText("resources/LICENSE");
    char *finalText = "\n\n\n\n\n\n\n\n\n\n\nThank you for playing!\n\n= Press ENTER to restart =\n\n\n\n\n\n\n";

    char *endingText = malloc(sizeof(char)*(strlen(creditText) + strlen(statsText) +
            strlen(license) + strlen(finalText) + 5));
    assert(endingText != NULL);

    endingText[0] = '\0';
    strcat(endingText, creditText);
    strcat(endingText, statsText);
    strcat(endingText, license);
    strcat(endingText, finalText);

    UnloadFileText(creditText);
    free(statsText);
    UnloadFileText(license);

    endingTextP = endingText;

    for (int i = 0; i < ENDING_SHOWING_LINES; i++)
        showingLines[i] = NULL;

    lineStart = -ENDING_SHOWING_LINES;

    framesCounter = 0;
    finishScreen = 0;
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{
    if (framesCounter%LINE_APPEAR_DELAY == 0 && endingTextP[0] != '\0')
    {
        // Shift lines showing
        for (int i = 0; i < ENDING_SHOWING_LINES - 1; i++)
            showingLines[i] = showingLines[i + 1];

        // Determinate the following line and move ending text pointer
        showingLines[ENDING_SHOWING_LINES - 1] = endingTextP;
        lineStart++;

        while (endingTextP[0] != '\n' && endingTextP[0] != '\0')
            endingTextP++;

        if (endingTextP[0] == '\n')
        {
            endingTextP[0] = '\0';
            endingTextP++;
        }
    }

    // Press enter or tap to return to TITLE screen
    if (framesCounter > 100 && IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

    framesCounter++;
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, BLACK);
    DrawCircleGradient(screenW/2, screenH/2, screenW*0.7, DARKGRAY, BLACK);

    DrawTextureTiledFill(floorTextureCredits,
            (Rectangle){0, 0, floorTextureCredits.width, floorTextureCredits.height},
            (Vector2){0, 0}, WHITE);

    const int FONT_SIZE = 20;

    for (int i = 0; i < ENDING_SHOWING_LINES; i++)
    {
        if (showingLines[i] == NULL) continue;

        int textW = MeasureText(showingLines[i], FONT_SIZE);
        int textX = (screenW - textW)/2.0;
        int textY = screenH*(i + 2)/(ENDING_SHOWING_LINES + 5);

        float phaseProgress = ((framesCounter+LINE_APPEAR_DELAY-1)%LINE_APPEAR_DELAY)/(float)LINE_APPEAR_DELAY;
        if (endingTextP[0]=='\0') phaseProgress = 0;
        textY -= phaseProgress*screenH/(ENDING_SHOWING_LINES + 5);

        Color col = WHITE;
        if (i == 0) col = Fade(col, 1 - phaseProgress);
        if (i == ENDING_SHOWING_LINES - 1) col = Fade(col, phaseProgress);

        int titleLevel = 0;
        if (strlen(showingLines[i]) >= 3)
        {
            titleLevel += (showingLines[i][0] == '=');
            titleLevel += (showingLines[i][1] == '=');
            titleLevel += (showingLines[i][2] == '=');
        }

        DrawText(showingLines[i], textX, textY, FONT_SIZE, col);
        if (titleLevel)
        {
            int moveX = rand()%(titleLevel*2 + 1) - titleLevel;
            int moveY = rand()%(titleLevel*2 + 1) - titleLevel;
            DrawText(showingLines[i], textX + moveX, textY + moveY, FONT_SIZE, col);
            DrawText(showingLines[i], textX - moveX, textY - moveY, FONT_SIZE, col);
        }
    }

}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    free(endingText);
    UnloadTexture(floorTextureCredits);
}

// Ending Screen should finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}
