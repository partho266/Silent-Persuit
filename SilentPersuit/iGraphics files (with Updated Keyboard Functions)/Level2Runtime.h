#ifndef LEVEL2_RUNTIME_H
#define LEVEL2_RUNTIME_H

#include "Level2Support.h"
#include "Level2.h"

// Level 2 player, input, camera, and shooting. This runtime is
// separate from Level 1's update loop and Levels 3 and 4.

bool level2StairTravelActive = false;
bool level2TKeyWasHeld = false;
double level2StairStartFloor = 0.0;
bool level2StairGoingUp = true;
bool level2StairAscendsRight = true;

double level2FloorUnderPlayer()
{
    if (assassin.y >= LEVEL2_FLOOR_4_Y - 25.0) return LEVEL2_FLOOR_4_Y;
    if (assassin.y >= LEVEL2_FLOOR_3_Y - 25.0) return LEVEL2_FLOOR_3_Y;
    if (assassin.y >= LEVEL2_FLOOR_2_Y - 25.0) return LEVEL2_FLOOR_2_Y;
    return LEVEL2_FLOOR_1_Y;
}

bool level2IsNearStairs()
{
    const double margin = 35.0;
    return (assassin.x >= STAIR1_X0 - margin && assassin.x <= STAIR1_X1 + margin) ||
           (assassin.x >= STAIR2_X0 - margin && assassin.x <= STAIR2_X1 + margin) ||
           (assassin.x >= STAIR3_X0 - margin && assassin.x <= STAIR3_X1 + margin);
}

void updateLevel2StairAccess()
{
    bool tHeld = isKeyPressed('t') || isKeyPressed('T');
    if (tHeld && !level2TKeyWasHeld && level2IsNearStairs())
    {
        level2StairTravelActive = true;
        level2StairStartFloor = level2FloorUnderPlayer();
        bool movingRight = (assassin.dx > 0.0) ||
                           (assassin.dx == 0.0 && assassin.facingRight);
        bool atRightStair = assassin.x >= STAIR3_X0 - 35.0 &&
                            assassin.x <= STAIR3_X1 + 35.0;
        level2StairAscendsRight = !atRightStair;
        level2StairGoingUp = level2StairAscendsRight ? movingRight : !movingRight;
    }
    if (!level2IsNearStairs())
    {
        level2StairTravelActive = false;
        level2StairStartFloor = 0.0;
    }
    level2TKeyWasHeld = tHeld;
}

double level2StairSurface(double x0, double x1)
{
    double from = level2StairStartFloor;
    double next = from;
    if (level2StairGoingUp)
    {
        if (from == LEVEL2_FLOOR_1_Y) next = LEVEL2_FLOOR_2_Y;
        else if (from == LEVEL2_FLOOR_2_Y) next = LEVEL2_FLOOR_3_Y;
        else if (from == LEVEL2_FLOOR_3_Y) next = LEVEL2_FLOOR_4_Y;
        if (level2StairAscendsRight)
            return from + (next - from) * (assassin.x - x0) / (x1 - x0);
        return next + (from - next) * (assassin.x - x0) / (x1 - x0);
    }

    if (from == LEVEL2_FLOOR_4_Y) next = LEVEL2_FLOOR_3_Y;
    else if (from == LEVEL2_FLOOR_3_Y) next = LEVEL2_FLOOR_2_Y;
    else if (from == LEVEL2_FLOOR_2_Y) next = LEVEL2_FLOOR_1_Y;
    if (level2StairAscendsRight)
        return next + (from - next) * (assassin.x - x0) / (x1 - x0);
    return from + (next - from) * (assassin.x - x0) / (x1 - x0);
}

double level2CeilingAbovePlayer()
{
    double floor = level2FloorUnderPlayer();
    if (floor == LEVEL2_FLOOR_1_Y) return LEVEL2_FLOOR_2_Y - LEVEL2_CEILING_CLEARANCE;
    if (floor == LEVEL2_FLOOR_2_Y) return LEVEL2_FLOOR_3_Y - LEVEL2_CEILING_CLEARANCE;
    if (floor == LEVEL2_FLOOR_3_Y) return LEVEL2_FLOOR_4_Y - LEVEL2_CEILING_CLEARANCE;
    return WIN_H - LEVEL2_CEILING_CLEARANCE;
}

double level2SurfaceHeightUnderPlayer()
{
    if (level2StairTravelActive)
    {
        if (assassin.x >= STAIR1_X0 && assassin.x <= STAIR1_X1)
            return level2StairSurface(STAIR1_X0, STAIR1_X1);
        if (assassin.x >= STAIR2_X0 && assassin.x <= STAIR2_X1)
            return level2StairSurface(STAIR2_X0, STAIR2_X1);
        if (assassin.x >= STAIR3_X0 && assassin.x <= STAIR3_X1)
            return level2StairSurface(STAIR3_X0, STAIR3_X1);
    }
    return level2FloorUnderPlayer();
}

void level2ApplyHorizontalMovement(bool leftHeld, bool rightHeld)
{
    if (leftHeld && !rightHeld)       assassin.dx = -3.0;
    else if (rightHeld && !leftHeld)  assassin.dx = 3.0;
    else if (assassin.isJumping)      assassin.dx = assassin.jumpDx;
    else                              assassin.dx = 0.0;
    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 15.0, LEVEL2_WORLD_WIDTH - 15.0);
}

void level2ApplyGravityAndLanding()
{
    double surface = level2SurfaceHeightUnderPlayer();
    if (level2StairTravelActive)
    {
        assassin.y = surface;
        assassin.dy = 0.0;
        assassin.isJumping = false;
        return;
    }

    if (assassin.y > level2CeilingAbovePlayer() && assassin.dy > 0.0)
    {
        assassin.y = level2CeilingAbovePlayer();
        assassin.dy = 0.0;
    }
    assassin.dy -= GRAVITY;
    assassin.y += assassin.dy;
    if (assassin.y <= surface)
    {
        assassin.y = surface;
        assassin.dy = 0.0;
        assassin.isJumping = false;
        assassin.jumpFrame = 0;
        assassin.jumpFrameTimer = 0.0;
    }
}

void level2TryJump(bool leftHeld, bool rightHeld)
{
    bool jumpPressed = isKeyPressed('w') || isKeyPressed('W') ||
                       isKeyPressed(' ') || isSpecialKeyPressed(GLUT_KEY_UP);
    if (!jumpPressed || assassin.isJumping) return;

    assassin.dy = 12.0;
    assassin.isJumping = true;
    assassin.jumpFrame = 0;
    assassin.jumpFrameTimer = 0.0;
    if (leftHeld && !rightHeld)       assassin.jumpDx = -4.0;
    else if (rightHeld && !leftHeld)  assassin.jumpDx = 4.0;
    else                              assassin.jumpDx = assassin.facingRight ? 4.0 : -4.0;
}

void level2FollowCamera(double targetWorldX)
{
    double cameraTarget = targetWorldX - WIN_W / 2.0;
    cameraTarget = clampD(cameraTarget, 0.0, LEVEL2_WORLD_WIDTH - WIN_W);
    cameraX += (cameraTarget - cameraX) * CAMERA_FOLLOW;
}

Enemy* findLevel2ShotTarget(int aimX, int aimY, int& outZone)
{
    Enemy* best = NULL;
    outZone = HIT_NOTHING;
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
    {
        Enemy* e = &level2WaveEnemies[i];
        if (level2FloorIndex(e->y) != level2FloorIndex(assassin.y)) continue;
        int zone = enemyHitZone(*e, aimX, aimY);
        if (zone > outZone) { outZone = zone; best = e; }
    }
    return best;
}

void fireLevel2PlayerShot()
{
    muzzleFlashTimer = MUZZLE_FLASH_DURATION;
    fireCooldownTimer = FIRE_COOLDOWN;
    playerRecoilTimer = PLAYER_RECOIL_DURATION;
    if (checkLevel2MannequinShot(crosshairX, crosshairY)) return;
    if (checkLevel2CylinderShot(crosshairX, crosshairY)) return;

    int zone = HIT_NOTHING;
    Enemy* target = findLevel2ShotTarget(crosshairX, crosshairY, zone);
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

void updateLevel2Firing()
{
    countDown(fireCooldownTimer);
    bool canFire = isFiring && gunEquipped &&
                   !assassin.isJumping && fireCooldownTimer <= 0.0;
    if (canFire) fireLevel2PlayerShot();
    countDown(muzzleFlashTimer);
    countDown(playerRecoilTimer);
    countDown(hitMarkTimer);
    countDown(headshotTextTimer);
    countDown(mouseActiveTimer);
}

void resetLevel2()
{
    currentLevel = 2;
    assassin.x = 120.0;
    assassin.y = LEVEL2_FLOOR_1_Y;
    assassin.dx = assassin.dy = 0.0;
    assassin.isJumping = false;
    assassin.facingRight = true;
    assassin.walkFrame = 0;
    assassin.walkFrameTimer = 0.0;
    assassin.jumpFrame = 0;
    assassin.jumpFrameTimer = 0.0;
    assassin.jumpDx = 0.0;

    cameraX = 0.0;
    shakeTimer = shakeOffsetX = 0.0;
    gunEquipped = false;
    isRpgEquipped = false;
    isKatanaEquipped = false;
    isKnifeEquipped = false;
    isHiding = false;
    isKatanaAttacking = false;
    isKatanaHeld = false;
    hasPlayerDealtDamageThisSwing = false;
    gKeyWasHeld = false;
    fKeyWasHeld = false;
    movementAKeyDown = movementDKeyDown = false;
    isFiring = false;
    muzzleFlashTimer = fireCooldownTimer = playerRecoilTimer = 0.0;
    playerHP = PLAYER_MAX_HP;
    playerEP = 0;
    epConversionTimer = 0.0;
    epConversionPaused = false;
    hitMarkTimer = headshotTextTimer = 0.0;

    level2StairTravelActive = false;
    level2TKeyWasHeld = false;
    level2StairStartFloor = 0.0;
    level2StairGoingUp = true;
    level2StairAscendsRight = true;
    resetLevel2AutoHeal();

    resetLevel2Smoke();
    resetLevel2Waves();
}

void updateLevel2()
{
    bool leftHeld = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

    toggleGunWithGKey();
    updateLevel2StairAccess();
    level2ApplyHorizontalMovement(leftHeld, rightHeld);
    updateShake();
    level2FollowCamera(assassin.x);
    level2ApplyGravityAndLanding();
    level2TryJump(leftHeld, rightHeld);
    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();
    updatePlayerJumpAnimation();

    updateLevel2Systems();
    updateLevel2Firing();
    if (playerHP <= 0) state = GAME_OVER;
}

void drawLevel2Background()
{
    for (int i = 0; i < BUILDING_TILE_COUNT; i++)
        iShowImage((int)worldToScreen(i * WIN_W), 0, WIN_W, WIN_H, buildingFloorTextures[i]);
}

void drawLevel2Hud()
{
    iSetColor(255, 255, 255);
    iText(15, WIN_H - 30, "LEVEL 2: The Building", GLUT_BITMAP_HELVETICA_18);
    char hpBuf[48];
    sprintf(hpBuf, "HP: %d / %d", playerHP, PLAYER_MAX_HP);
    iText(15, WIN_H - 52, hpBuf, GLUT_BITMAP_HELVETICA_18);
    iText(15, WIN_H - 72, "A/D move | G gun | T near stairs | W/Space jump | Left-click fire", GLUT_BITMAP_HELVETICA_12);
    char progressBuf[96];
    sprintf(progressBuf, "Keys: %d / 2 | Enemies defeated: %d / %d", collectedKeyCount, enemiesKilled, LEVEL2_TOTAL_ENEMIES);
    iText(15, WIN_H - 92, progressBuf, GLUT_BITMAP_HELVETICA_12);
    if (smokeReleased)
    {
        sprintf(progressBuf, "Smoke active: %d seconds", (int)smokeTimer + 1);
        iText(15, WIN_H - 110, progressBuf, GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        sprintf(progressBuf, "Gas cylinder: %d / %d hits", cylinderHitCount, CYLINDER_HITS_TO_RELEASE_SMOKE);
        iText(15, WIN_H - 110, progressBuf, GLUT_BITMAP_HELVETICA_12);
    }
    if (tenKillDecision)
        iText(15, WIN_H - 128, "Shoot the gas cylinder before the timer runs out!", GLUT_BITMAP_HELVETICA_12);
    else if (level2IsNearStairs())
        iText(15, WIN_H - 128, "Press T to use the stairs.", GLUT_BITMAP_HELVETICA_12);
    else
        iText(15, WIN_H - 128, "Explore all four floors of the building.", GLUT_BITMAP_HELVETICA_12);
}

void drawLevel2Screen()
{
    drawLevel2Background();
    drawLevel2Systems();
    drawPlayer();
    drawHitMarker();
    drawCrosshair();
    drawLevel2Hud();

    if (level2ExitActivated)
    {
        beginTransparency();
        setColorAlpha(0, 0, 0, 0.62);
        iFilledRectangle(0, 150, WIN_W, 260);
        endTransparency();

        iSetColor(255, 215, 80);
        iText(WIN_W / 2 - 190, 330, "LEVEL 2 COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 245, 190);
        iText(WIN_W / 2 - 235, 285, "THE KEYS HAVE BEEN COLLECTED", GLUT_BITMAP_HELVETICA_18);
        iSetColor(230, 210, 175);
        iText(WIN_W / 2 - 195, 245, "Press SPACE to continue.", GLUT_BITMAP_HELVETICA_18);
    }
}

void drawLevel2GameOverScreen()
{
    drawLevel2Background();
    beginTransparency();
    setColorAlpha(0, 0, 0, 0.72);
    iFilledRectangle(0, 0, WIN_W, WIN_H);
    endTransparency();
    iSetColor(220, 40, 30);
    iText(WIN_W / 2 - 95, 430, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(220, 210, 200);
    iText(WIN_W / 2 - 105, 390, "You were killed in combat.", GLUT_BITMAP_HELVETICA_18);
    drawButton(btnRetry);
    drawButton(btnGoToMenu);
}

#endif
