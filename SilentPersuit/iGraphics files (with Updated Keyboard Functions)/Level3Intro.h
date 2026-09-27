#ifndef LEVEL3_INTRO_H
#define LEVEL3_INTRO_H

// =========================================================
// SILENT PURSUIT - Level3Intro.h
// ---------------------------------------------------------
// The short cinematic that plays before Level 3 itself:
// three establishing shots of the ruined temple, one after
// another. Each shot fades in from black, holds with a slow
// drifting zoom (so it never feels like a frozen photo), then
// fades back to black before the next one appears.
//
// This file only tracks WHEN we are in the sequence - which
// shot, how far into it, whether it's finished. The actual
// drawing (drawLevel3IntroScreen) lives in Screens.h, same as
// every other screen in the game.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Assets.h"   // for LEVEL3_INTRO_SHOT_COUNT
#include "Audio.h"


// ---------------------------------------------------------
// 1. TIMING (how long each shot spends in each phase)
// ---------------------------------------------------------

const double L3_FADE_IN_TIME = 1.0;
const double L3_HOLD_TIME = 3.0;
const double L3_FADE_OUT_TIME = 1.0;

const double L3_SHOT_TIME = L3_FADE_IN_TIME + L3_HOLD_TIME + L3_FADE_OUT_TIME;

// How much the picture slowly zooms in while it plays, so it
// reads as a slow camera push rather than a still image.
const double L3_ZOOM_START = 1.00;
const double L3_ZOOM_END = 1.12;


// ---------------------------------------------------------
// 2. STATE
// ---------------------------------------------------------

int    level3IntroShot = 0;       // which of the 3 pictures we're on (0-2)
double level3IntroTimer = 0.0;    // seconds spent on the current picture
bool   level3IntroDone = false;   // true once iMain.cpp should move to LEVEL3
bool   level3IntroAudioPlaying = false;


// ---------------------------------------------------------
// 3. START THE CINEMATIC (called when Level 3 is chosen)
// ---------------------------------------------------------

void resetLevel3Intro()
{
    level3IntroShot = 0;
    level3IntroTimer = 0.0;
    level3IntroDone = false;
    playGameAudioChannel("LEVEL3_INTRO.mp3", "level3_intro_audio", true);
    level3IntroAudioPlaying = true;
}


// ---------------------------------------------------------
// 4. ADVANCE THE TIMER (called every tick from iMain.cpp)
// ---------------------------------------------------------

void updateLevel3Intro()
{
    if (level3IntroDone) return;

    level3IntroTimer += TICK;

    if (level3IntroTimer >= L3_SHOT_TIME)
    {
        level3IntroTimer = 0.0;
        level3IntroShot++;

        if (level3IntroShot >= LEVEL3_INTRO_SHOT_COUNT)
        {
            level3IntroDone = true;
            stopGameAudioChannel("level3_intro_audio");
            level3IntroAudioPlaying = false;
        }
    }
}


// ---------------------------------------------------------
// 5. SKIP (a click or a key press jumps straight to Level 3)
// ---------------------------------------------------------

void skipLevel3Intro()
{
    level3IntroDone = true;
    stopGameAudioChannel("level3_intro_audio");
    level3IntroAudioPlaying = false;
}

#endif
