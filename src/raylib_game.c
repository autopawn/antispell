/*******************************************************************************************
*
*   raylib game template
*
*   Antispell
*   A game about a mage that has to absorv its enemies' attacks to crats spells on the fly!
*   Made for the raylib 5K gamejam.
*   Made by Francisco Casas (https://fcasas.itch.io/)
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Total number of levels
#define N_LEVELS 2

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GameScreen currentScreen = 0;
Font font = { 0 };
Music music[4] = { 0 };
Sound fxCoin = { 0 };

static int currentMusic = -1;
static int nextMusic = 0; // Change this variable to change the music

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 600;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

static int currentLevel = 0;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Antispell");

    InitAudioDevice();      // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    fxCoin = LoadSound("resources/coin.wav");

    music[0] = LoadMusicStream("resources/music/gothamlicious-32.mp3");
    music[1] = LoadMusicStream("resources/music/ether_vox-32.mp3");
    music[2] = LoadMusicStream("resources/music/night_vigil-32.mp3");
    music[3] = LoadMusicStream("resources/music/hot_pursuit-32.mp3");

    // Setup and init first screen
    #ifdef _DEBUG
        currentScreen = GAMESCREEN_GAMEPLAY;
        currentLevel = 0;
        InitGameplayScreen(currentLevel);
    #else
        currentLevel = 1;
        InitLogoScreen();
    #endif

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 48, 1);
#else
    SetTargetFPS(48);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (currentScreen)
    {
        case GAMESCREEN_LOGO: UnloadLogoScreen(); break;
        case GAMESCREEN_TITLE: UnloadTitleScreen(); break;
        case GAMESCREEN_GAMEPLAY: UnloadGameplayScreen(); break;
        case GAMESCREEN_ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music[0]);
    UnloadMusicStream(music[1]);
    UnloadMusicStream(music[2]);
    UnloadMusicStream(music[3]);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen, no transition
static void ChangeToScreen(int screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case GAMESCREEN_LOGO: UnloadLogoScreen(); break;
        case GAMESCREEN_TITLE: UnloadTitleScreen(); break;
        case GAMESCREEN_GAMEPLAY: UnloadGameplayScreen(); break;
        case GAMESCREEN_ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case GAMESCREEN_LOGO: InitLogoScreen(); break;
        case GAMESCREEN_TITLE: InitTitleScreen(); break;
        case GAMESCREEN_GAMEPLAY: InitGameplayScreen(currentLevel); break;
        case GAMESCREEN_ENDING: InitEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(int screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect (fade-in, fade-out)
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
        // For that reason we compare against 1.01f, to avoid last frame loading stop
        if (transAlpha > 1.01f)
        {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen)
            {
                case GAMESCREEN_LOGO: UnloadLogoScreen(); break;
                case GAMESCREEN_TITLE: UnloadTitleScreen(); break;
                case GAMESCREEN_OPTIONS: UnloadOptionsScreen(); break;
                case GAMESCREEN_GAMEPLAY: UnloadGameplayScreen(); break;
                case GAMESCREEN_ENDING: UnloadEndingScreen(); break;
                default: break;
            }

            // Load next screen
            switch (transToScreen)
            {
                case GAMESCREEN_LOGO: InitLogoScreen(); break;
                case GAMESCREEN_TITLE: InitTitleScreen(); break;
                case GAMESCREEN_GAMEPLAY: InitGameplayScreen(currentLevel); break;
                case GAMESCREEN_ENDING: InitEndingScreen(); break;
                default: break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;

        if (transAlpha < -0.01f)
        {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    if (currentMusic != nextMusic)
    {
        if (currentMusic != -1)
            StopMusicStream(music[currentMusic]);

        currentMusic = nextMusic;
        PlayMusicStream(music[currentMusic]);
        SetMusicVolume(music[currentMusic], 1.0f);
    }
    UpdateMusicStream(music[currentMusic]);       // NOTE: Music keeps playing between screens

    if (!onTransition)
    {
        switch(currentScreen)
        {
            case GAMESCREEN_LOGO:
            {
                nextMusic = 0;

                // Faster!
                UpdateLogoScreen();
                UpdateLogoScreen();
                UpdateLogoScreen();

                if (FinishLogoScreen()) TransitionToScreen(GAMESCREEN_TITLE);

            } break;
            case GAMESCREEN_TITLE:
            {
                nextMusic = 0;
                UpdateTitleScreen();

                if (FinishTitleScreen() == 1) TransitionToScreen(GAMESCREEN_OPTIONS);
                else if (FinishTitleScreen() == 2) TransitionToScreen(GAMESCREEN_GAMEPLAY);

            } break;
            case GAMESCREEN_OPTIONS:
            {
                nextMusic = 0;
                UpdateOptionsScreen();

                if (FinishOptionsScreen()) TransitionToScreen(GAMESCREEN_TITLE);

            } break;
            case GAMESCREEN_GAMEPLAY:
            {
                nextMusic = 1;
                if (currentLevel > N_LEVELS/2) nextMusic = 2;
                if (currentLevel == N_LEVELS) nextMusic = 3;

                UpdateGameplayScreen();

                int result = FinishGameplayScreen();

                if (result > 0)
                {
                    if (result == 2) currentLevel++;
                    if (currentLevel > N_LEVELS)
                    {
                        currentLevel = 1;
                        TransitionToScreen(GAMESCREEN_ENDING);
                    }
                    else
                    {
                        TransitionToScreen(GAMESCREEN_GAMEPLAY);
                    }
                }
                //else if (FinishGameplayScreen() == 2) TransitionToScreen(GAMESCREEN_TITLE);

            } break;
            case GAMESCREEN_ENDING:
            {
                nextMusic = 0;
                UpdateEndingScreen();

                if (FinishEndingScreen() == 1) TransitionToScreen(GAMESCREEN_TITLE);

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch(currentScreen)
        {
            case GAMESCREEN_LOGO: DrawLogoScreen(); break;
            case GAMESCREEN_TITLE: DrawTitleScreen(); break;
            case GAMESCREEN_OPTIONS: DrawOptionsScreen(); break;
            case GAMESCREEN_GAMEPLAY: DrawGameplayScreen(); break;
            case GAMESCREEN_ENDING: DrawEndingScreen(); break;
            default: break;
        }

        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();

        //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
