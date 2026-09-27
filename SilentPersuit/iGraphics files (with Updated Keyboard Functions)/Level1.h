#ifndef LEVEL1_H
#define LEVEL1_H

// =========================================================
// SILENT PURSUIT - Level1.h
// ---------------------------------------------------------
// All the RULES of Level 1 live here: reading the keyboard,
// moving the assassin, bumping into things, gravity, firing
// the gun and checking whether the mission is lost.
//
// updateLevel1() is called 50 times a second from the timer
// in iMain.cpp. It is split into small steps so each one can
// be read on its own.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Assets.h"
#include "World.h"
#include "Effects.h"
#include "UI.h"
#include "Player.h"
#include "Enemy.h"
#include "Landmine.h"


// ---------------------------------------------------------
// 1. HEADSHOT MESSAGE
// ---------------------------------------------------------

double headshotTextTimer = 0.0;
const double HEADSHOT_TEXT_DURATION = 0.9;
bool level1Complete = false;


// ---------------------------------------------------------
// 2. START / RESTART THE LEVEL
// ---------------------------------------------------------

void resetLevel1()
{
    assassin.x = 200.0;
    assassin.y = GROUND_Y;
    assassin.dx = 0.0;
    assassin.dy = 0.0;
    assassin.isJumping = false;
    assassin.facingRight = true;
    assassin.walkFrame = 0;
    assassin.walkFrameTimer = 0.0;
    assassin.jumpFrame = 0;
    assassin.jumpFrameTimer = 0.0;
    assassin.jumpDx = 0.0;

    cameraX = 0.0;
    shakeTimer = 0.0;
    shakeOffsetX = 0.0;

    gunEquipped = false;
    isRpgEquipped = false;   // the RPG only exists in Level 3
    isFiring = false;
    muzzleFlashTimer = 0.0;
    fireCooldownTimer = 0.0;
    playerRecoilTimer = 0.0;

    playerHP = PLAYER_MAX_HP;

    hitMarkTimer = 0.0;
    headshotTextTimer = 0.0;
    level1Complete = false;

    resetLandmine();
    resetAllEnemies();
}


// ---------------------------------------------------------
// 3. KEYBOARD -> MOVEMENT
// ---------------------------------------------------------

void applyHorizontalMovement(bool leftHeld, bool rightHeld)
{
    if (leftHeld && !rightHeld)       assassin.dx = -3.0;
    else if (rightHeld && !leftHeld)  assassin.dx = 3.0;
    else if (assassin.isJumping)      assassin.dx = assassin.jumpDx;
    else                              assassin.dx = 0.0;

    assassin.x += assassin.dx;
}

// toggleGunWithGKey() now lives in Player.h, alongside
// toggleRpgWithFKey() - both are Player state, and Level 3
// needs to call the gun one too.


// ---------------------------------------------------------
// 4. WALLS AND EDGES
// ---------------------------------------------------------

// Pushes the player back out of a solid block he walked into.
void blockSideways(double blockX0, double blockX1, double blockTopY)
{
    // If he is above the block he is standing on it, not
    // walking into it.
    if (assassin.y >= blockTopY - 25.0) return;

    if (assassin.x > blockX0 - 20.0 && assassin.x < blockX1 + 20.0)
    {
        if (assassin.dx > 0.0)      assassin.x = blockX0 - 20.0;
        else if (assassin.dx < 0.0) assassin.x = blockX1 + 20.0;
    }
}

void applyLevelBoundaries()
{
    // The gate stays shut until the second guard is down.
    if (enemy2.alive && assassin.x > SWITCH_X) assassin.x = SWITCH_X;

    // The rock and the concrete block are solid.
    blockSideways(OBSTACLE_X0, OBSTACLE_X1, OBSTACLE_TOP_Y);

    double blockTop = groundYAt(ENEMY_BLOCK_X) + ENEMY_BLOCK_H;
    blockSideways(ENEMY_BLOCK_X, ENEMY_BLOCK_X + ENEMY_BLOCK_W, blockTop);

    // Never walk off the edges of the map.
    assassin.x = clampD(assassin.x, 15.0, WORLD_WIDTH - 15.0);
}


// ---------------------------------------------------------
// 5. GRAVITY AND STANDING ON THINGS
// ---------------------------------------------------------

// The height the player would land on at his current X.
// Normally the ground, but the rock and the block are higher.
double surfaceHeightUnderPlayer()
{
    double surface = groundYAt(assassin.x);

    double blockTop = groundYAt(ENEMY_BLOCK_X) + ENEMY_BLOCK_H;
    if (assassin.x > ENEMY_BLOCK_X - 20.0 &&
        assassin.x < ENEMY_BLOCK_X + ENEMY_BLOCK_W + 20.0)
    {
        if (assassin.dy <= 0.0 && assassin.y >= blockTop - 30.0) surface = blockTop;
    }

    if (assassin.x > OBSTACLE_X0 - 20.0 && assassin.x < OBSTACLE_X1 + 20.0)
    {
        if (assassin.dy <= 0.0 && assassin.y >= OBSTACLE_TOP_Y - 30.0) surface = OBSTACLE_TOP_Y;
    }

    return surface;
}

void applyGravityAndLanding()
{
    double surface = surfaceHeightUnderPlayer();
    bool overGap = isOverGap(assassin.x);

    bool onGround = (assassin.y <= surface) && !overGap;

    if (!onGround) assassin.dy -= GRAVITY;
    assassin.y += assassin.dy;

    if (assassin.y <= surface && !overGap)
    {
        // landed
        assassin.y = surface;
        assassin.dy = 0.0;
        assassin.isJumping = false;
        assassin.jumpFrame = 0;
        assassin.jumpFrameTimer = 0.0;
    }
    else if (assassin.y <= surface && overGap)
    {
        // falling through the broken bridge
        if (!assassin.isJumping)
        {
            assassin.isJumping = true;
            assassin.jumpFrame = JUMP_FRAME_COUNT - 1;
        }

        if (assassin.y < FALL_DEATH_Y) state = GAME_OVER;
    }
}

void tryJump(bool leftHeld, bool rightHeld)
{
    bool jumpPressed = isKeyPressed('w') || isKeyPressed('W') ||
                       isKeyPressed(' ') || isSpecialKeyPressed(GLUT_KEY_UP);

    if (!jumpPressed || assassin.isJumping) return;

    assassin.dy = 12.0;
    assassin.isJumping = true;
    assassin.jumpFrame = 0;
    assassin.jumpFrameTimer = 0.0;

    // The sideways speed is locked in at take-off.
    if (leftHeld && !rightHeld)       assassin.jumpDx = -4.0;
    else if (rightHeld && !leftHeld)  assassin.jumpDx = 4.0;
    else                              assassin.jumpDx = assassin.facingRight ? 4.0 : -4.0;
}


// ---------------------------------------------------------
// 6. WAKING THE GUARDS UP
// ---------------------------------------------------------
// A guard only starts acting once he comes into view, so the
// player is not shot at from off-screen.
// ---------------------------------------------------------

void wakeEnemiesInView()
{
    double rightEdge = cameraX + WIN_W;

    if (!enemy1.active && !enemy1.corpseGone && rightEdge > ENEMY_BLOCK_X + 20.0)
        enemy1.active = true;

    if (!enemy2.active && !enemy2.corpseGone && rightEdge > ENEMY2_X)
        enemy2.active = true;
}


// ---------------------------------------------------------
// 7. FIRING THE GUN
// ---------------------------------------------------------

// Looks at every guard and returns the best thing the bullet
// hit. A head counts as better than a body, so if the two
// areas ever overlap the headshot wins.
Enemy* findShotTarget(int aimX, int aimY, int& outZone)
{
    Enemy* best = NULL;
    outZone = HIT_NOTHING;

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        Enemy* e = allEnemies[i];

        int zone = enemyHitZone(*e, aimX, aimY);

        if (zone > outZone)
        {
            outZone = zone;
            best = e;
        }
    }

    return best;
}

void firePlayerShot()
{
    muzzleFlashTimer = MUZZLE_FLASH_DURATION;
    fireCooldownTimer = FIRE_COOLDOWN;
    playerRecoilTimer = PLAYER_RECOIL_DURATION;
    // 1. Did the bullet hit the landmine?
    if (checkLandmineShot(crosshairX, crosshairY)) return;

    // 2. Did it hit a guard?
    int zone = HIT_NOTHING;
    Enemy* target = findShotTarget(crosshairX, crosshairY, zone);

    if (target == NULL || zone == HIT_NOTHING) return;

    if (zone == HIT_HEAD)
    {
        damageEnemy(*target, PLAYER_HEADSHOT_DAMAGE);
        headshotTextTimer = HEADSHOT_TEXT_DURATION;
        showHitMarker(crosshairX, crosshairY, true);
    }
    else
    {
        damageEnemy(*target, PLAYER_BODY_DAMAGE);
        showHitMarker(crosshairX, crosshairY, false);
    }
}

void updateFiring()
{
    countDown(fireCooldownTimer);

    bool canFire = isFiring && gunEquipped &&
                   !assassin.isJumping && fireCooldownTimer <= 0.0;

    if (canFire) firePlayerShot();

    countDown(muzzleFlashTimer);
    countDown(playerRecoilTimer);
    countDown(hitMarkTimer);
    countDown(headshotTextTimer);
    countDown(mouseActiveTimer);
}


// ---------------------------------------------------------
// 8. ONE FULL STEP OF THE LEVEL
// ---------------------------------------------------------

void updateLevel1()
{
    bool leftHeld  = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

    toggleGunWithGKey();

    applyHorizontalMovement(leftHeld, rightHeld);
    applyLevelBoundaries();

    checkLandmineStep();
    updateLandmine();
    updateShake();

    followCamera(assassin.x);

    applyGravityAndLanding();
    tryJump(leftHeld, rightHeld);

    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();
    updatePlayerJumpAnimation();

    wakeEnemiesInView();
    updateEnemy(enemy1, true);
    updateEnemy(enemy2, !enemy1.alive);   // guard 2 waits his turn

    updateFiring();

    // Shooting the landmine from a safe distance completes Level 1.
    // Stepping on it remains a failure and must not show the completion UI.
    if (landmineTriggered && !killedByMine && !level1Complete && playerHP > 0)
    {
        level1Complete = true;
        playGameAudio("LEVEL_COMPLETED.mp3");
    }

    if (playerHP <= 0) state = GAME_OVER;
}

#endif
