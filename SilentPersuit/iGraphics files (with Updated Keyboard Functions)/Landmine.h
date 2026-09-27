#ifndef LANDMINE_H
#define LANDMINE_H

// =========================================================
// SILENT PURSUIT - Landmine.h
// ---------------------------------------------------------
// The hidden mine near the castle gates.
//
// NEW IN THIS VERSION - it really explodes now.
// It can go off in two ways:
//
//      1. the assassin STEPS on it   -> instant death
//      2. the assassin SHOOTS at it  -> it blows up where it
//         is, and how much it hurts depends on how close he
//         was standing:
//
//              inside the kill radius  -> death
//              inside the hurt radius  -> heavy damage
//              further away            -> safe, mine cleared
//
// Either way the same blast animation plays: white flash,
// shockwave ring, fireball, smoke, flying debris and a
// shaking camera.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Draw.h"
#include "World.h"
#include "Effects.h"
#include "Player.h"


// ---------------------------------------------------------
// 1. STATE
// ---------------------------------------------------------

bool landmineActive = true;        // still buried and armed
bool landmineTriggered = false;    // has it gone off
bool killedByMine = false;         // used for the Game Over text
double landmineTimer = 0.0;        // how much of the animation is left

double blastFlashTimer = 0.0;
const double BLAST_FLASH_DURATION = 0.18;


// ---------------------------------------------------------
// 2. BLAST SETTINGS
// ---------------------------------------------------------

const double MINE_STEP_RANGE = 15.0;    // how close you must be to step on it
const double MINE_AIM_RANGE = 40.0;     // how close the crosshair must be to hit it

const double MINE_KILL_RADIUS = 70.0;   // standing this close = dead
const double MINE_HURT_RADIUS = 190.0;  // standing this close = wounded
const int    MINE_BLAST_DAMAGE = 70;

const double MINE_SHAKE_TIME = 0.55;
const double MINE_SHAKE_POWER = 16.0;


// ---------------------------------------------------------
// 3. SETTING IT OFF
// ---------------------------------------------------------

void triggerLandmine(bool steppedOn)
{
    landmineActive = false;
    landmineTriggered = true;
    landmineTimer = EXPLOSION_DURATION;

    blastFlashTimer = BLAST_FLASH_DURATION;
    startShake(MINE_SHAKE_TIME, MINE_SHAKE_POWER);
    playGameAudioChannel("Landmine.mp3", "landmine_audio");

    if (steppedOn)
    {
        playerHP = 0;
        killedByMine = true;
        return;
    }

    // Shot from a distance: work out how far away he was.
    double mineY = groundYAt(MINE_X) + 5.0;
    double dist = distance2D(assassin.x, assassin.y, MINE_X, mineY);

    if (dist <= MINE_KILL_RADIUS)
    {
        playerHP = 0;
        killedByMine = true;
    }
    else if (dist <= MINE_HURT_RADIUS)
    {
        // The further out he is, the less it hurts.
        double closeness = 1.0 - ((dist - MINE_KILL_RADIUS) /
                                  (MINE_HURT_RADIUS - MINE_KILL_RADIUS));

        int damage = (int)(MINE_BLAST_DAMAGE * closeness);

        playerHP -= damage;
        if (playerHP < 0) playerHP = 0;
        if (playerHP == 0) killedByMine = true;
    }
    // Beyond the hurt radius he is simply safe.
}

// Called once per tick while the player is walking around.
void checkLandmineStep()
{
    if (!landmineActive) return;

    bool nearMine = (assassin.x >= MINE_X - MINE_STEP_RANGE &&
                     assassin.x <= MINE_X + MINE_STEP_RANGE);

    bool onTheGround = (assassin.y <= groundYAt(assassin.x) + 5.0);

    if (nearMine && onTheGround) triggerLandmine(true);
}

// Called once every time the player pulls the trigger.
// Returns true if the bullet hit the mine.
bool checkLandmineShot(int aimX, int aimY)
{
    if (!landmineActive) return false;

    double mineScreenX = worldToScreen(MINE_X);
    double mineScreenY = groundYAt(MINE_X) + 5.0;

    if (distance2D(aimX, aimY, mineScreenX, mineScreenY) > MINE_AIM_RANGE)
        return false;

    triggerLandmine(false);
    return true;
}

void updateLandmine()
{
    countDown(landmineTimer);
    countDown(blastFlashTimer);
}

void resetLandmine()
{
    landmineActive = true;
    landmineTriggered = false;
    killedByMine = false;
    landmineTimer = 0.0;
    blastFlashTimer = 0.0;
}


// ---------------------------------------------------------
// 4. DRAWING
// ---------------------------------------------------------

// The mine itself, while it is still armed.
void drawLandmine()
{
    if (!landmineActive) return;

    int x = (int)worldToScreen(MINE_X);
    int y = (int)groundYAt(MINE_X);

    iSetColor(60, 65, 60);                     // dark base plate
    iFilledRectangle(x - 12, y, 24, 5);

    iSetColor(90, 95, 90);                     // dome
    iFilledCircle(x, y + 5, 8);

    // The warning light blinks, using the global animTime.
    bool lightOn = (fmod(animTime, 1.0) < 0.5);
    if (lightOn) iSetColor(255, 60, 50);
    else         iSetColor(120, 25, 25);

    iFilledCircle(x, y + 11, 3);
}

// The explosion. Drawn after the characters so the smoke
// covers them.
void drawLandmineBlast()
{
    if (!landmineTriggered || landmineTimer <= 0.0) return;

    drawExplosion(worldToScreen(MINE_X), groundYAt(MINE_X), landmineTimer);
}

#endif
