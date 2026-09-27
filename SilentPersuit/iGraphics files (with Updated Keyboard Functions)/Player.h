#ifndef PLAYER_H
#define PLAYER_H

// =========================================================
// SILENT PURSUIT - Player.h
// =========================================================

#include "Config.h"
#include "Draw.h"
#include "Assets.h"
#include "World.h"
#include "Effects.h"
#include "UI.h"

// ---------------------------------------------------------
// 1. HEALTH, ENERGY & COMBAT TRACKING
// ---------------------------------------------------------
const int PLAYER_MAX_HP = 200;
int playerHP = PLAYER_MAX_HP;

// NEW: Energy Points logic (20 EP converts to 20 HP over 4 seconds)
int playerEP = 0;
double epConversionTimer = 0.0;
bool epConversionPaused = false;
const double EP_CONVERSION_RATE = 0.2; // 1 HP every 0.2 seconds (5 HP per sec)

void updateEP()
{
    if (epConversionPaused)
        return;

    if (playerEP > 0 && playerHP < PLAYER_MAX_HP)
    {
        epConversionTimer += TICK;
        if (epConversionTimer >= EP_CONVERSION_RATE)
        {
            epConversionTimer -= EP_CONVERSION_RATE;
            playerEP -= 1;
            playerHP += 1;
            if (playerHP > PLAYER_MAX_HP) playerHP = PLAYER_MAX_HP;
        }
    }
    else
    {
        epConversionTimer = 0.0; // Reset timer if full health or no EP
    }
}

bool gunEquipped = false;
bool isRpgEquipped = false;
bool isKatanaEquipped = false;
bool isKnifeEquipped = false;  
bool isHiding = false;

// KEY MECHANICS
bool hasSecurityKey = false;
bool isKeyDropped = false;
double keyX = 0.0;
double keyY = 0.0;
bool kKeyWasHeld = false;
bool xKeyWasHeld = false;

bool gKeyWasHeld = false;
bool fKeyWasHeld = false;

bool isFiring = false;
double muzzleFlashTimer = 0.0;

bool isKatanaAttacking = false;
bool isKatanaHeld = false;     
int katanaAttackFrame = 0;
double katanaAttackTimer = 0.0;
bool hasPlayerDealtDamageThisSwing = false;

const double FIRE_COOLDOWN = 0.5;
double fireCooldownTimer = 0.0;

const double PLAYER_RECOIL_DURATION = 0.1;
const double PLAYER_RECOIL_PIXELS = 6.0;
double playerRecoilTimer = 0.0;

int lastGunPoseIdx = GUN_NEUTRAL;

void toggleGunWithGKey()
{
    bool gHeld = isKeyPressed('g') || isKeyPressed('G');
    if (gHeld && !gKeyWasHeld)
    {
        gunEquipped = !gunEquipped;
        if (gunEquipped) { isRpgEquipped = false; isKatanaEquipped = false; }
        if (!gunEquipped) isFiring = false;
    }
    gKeyWasHeld = gHeld;
}

void toggleRpgWithFKey()
{
    bool fHeld = isKeyPressed('f') || isKeyPressed('F');
    if (fHeld && !fKeyWasHeld)
    {
        isRpgEquipped = !isRpgEquipped;
        if (isRpgEquipped) { gunEquipped = false; isKatanaEquipped = false; isFiring = false; }
    }
    fKeyWasHeld = fHeld;
}

struct Player
{
    double x, y;
    double dx, dy;
    bool isJumping;
    int width, height;
    bool facingRight;
    int walkFrame;
    double walkFrameTimer;
    int jumpFrame;
    double jumpFrameTimer;
    double jumpDx;
};

Player assassin = { 200.0, GROUND_Y, 0.0, 0.0, false, 30, 60, true, 0, 0.0, 0, 0.0, 0.0 };

bool playerIsMoving() { return (fabs(assassin.dx) > 0.5); }

int chooseGunPose()
{
    double shoulderY = assassin.y + SPRITE_Y_OFFSET + SPRITE_DRAW_H * 0.6;
    double dy = crosshairY - shoulderY;
    if (dy > AIM_TIER2)    return GUN_UP3;
    if (dy > AIM_TIER1)    return GUN_UP2;
    if (dy > AIM_DEADZONE) return GUN_UP1;
    if (dy < -AIM_TIER2)    return GUN_DOWN3;
    if (dy < -AIM_TIER1)    return GUN_DOWN2;
    if (dy < -AIM_DEADZONE) return GUN_DOWN1;
    return GUN_NEUTRAL;
}

void updatePlayerWalkAnimation()
{
    if (playerIsMoving() && !assassin.isJumping)
    {
        assassin.walkFrameTimer += TICK;
        if (assassin.walkFrameTimer >= WALK_FRAME_DURATION)
        {
            assassin.walkFrameTimer -= WALK_FRAME_DURATION;
            assassin.walkFrame++;

            int frameCount = WALK_FRAME_COUNT;
            if (isKatanaEquipped) frameCount = KATANA_RUN_FRAME_COUNT;
            else if (isKnifeEquipped) frameCount = KNIFE_WALK_FRAME_COUNT;
            else if (isRpgEquipped) frameCount = WALK_RPG_FRAME_COUNT;
            else if (gunEquipped) frameCount = WALK_GUN_FRAME_COUNT;

            if (assassin.walkFrame >= frameCount) assassin.walkFrame = 0;
        }
    }
    else
    {
        assassin.walkFrame = 0;
        assassin.walkFrameTimer = 0.0;
    }
}

void updatePlayerJumpAnimation()
{
    if (!assassin.isJumping) return;
    assassin.jumpFrameTimer += TICK;
    double frameDuration = gunEquipped ? (JUMP_FRAME_DURATION * JUMP_FRAME_COUNT / JUMP_GUN_FRAME_COUNT) : JUMP_FRAME_DURATION;
    if (assassin.jumpFrameTimer >= frameDuration)
    {
        assassin.jumpFrameTimer -= frameDuration;
        int lastFrame = (gunEquipped ? JUMP_GUN_FRAME_COUNT : JUMP_FRAME_COUNT) - 1;
        if (assassin.jumpFrame < lastFrame) assassin.jumpFrame++;
    }
}

void updatePlayerFacing(bool leftHeld, bool rightHeld)
{
    if (isKatanaEquipped || isKnifeEquipped)
    {
        if (leftHeld && !rightHeld) assassin.facingRight = false;
        else if (rightHeld && !leftHeld) assassin.facingRight = true;
        return;
    }

    if (isMouseActive())
    {
        double screenX = worldToScreen(assassin.x);
        assassin.facingRight = (crosshairX >= (int)screenX);
        return;
    }
    if (leftHeld && !rightHeld) { assassin.facingRight = false; return; }
    if (rightHeld && !leftHeld) { assassin.facingRight = true; return; }
    if (assassin.isJumping)
    {
        if (assassin.jumpDx > 0.5)       assassin.facingRight = true;
        else if (assassin.jumpDx < -0.5) assassin.facingRight = false;
        return;
    }
}

void pickPlayerSprite(unsigned int& sprite, int& drawW, int& drawH, double& yOffset, int& gunPose, bool& usingGunPose, int& hipOffsetX, bool& spriteIsPreOriented)
{
    sprite = 0;
    gunPose = GUN_NEUTRAL;
    usingGunPose = false;
    spriteIsPreOriented = false;   // true only for the knife's walk/attack frames (see below)

    if (isHiding && (isKatanaEquipped || isKnifeEquipped))
    {
        drawW = (int)(KATANA_IDLE_DRAW_W * 0.45);
        drawH = (int)(KATANA_IDLE_DRAW_H * 0.45);
        yOffset = KATANA_IDLE_Y_OFFSET - 30.0;
        hipOffsetX = drawW / 2;
        sprite = katanaAttackFrames[0];
        return;
    }

    if (state == LEVEL3 || state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4)
    {
        drawW = L3_SPRITE_DRAW_W; drawH = L3_SPRITE_DRAW_H; yOffset = L3_SPRITE_Y_OFFSET;
    }
    else
    {
        drawW = SPRITE_DRAW_W; drawH = SPRITE_DRAW_H; yOffset = SPRITE_Y_OFFSET;
    }
    hipOffsetX = drawW / 2;

    if (assassin.isJumping)
    {
        if (gunEquipped)
        {
            int f = clampI(assassin.jumpFrame, 0, JUMP_GUN_FRAME_COUNT - 1);
            sprite = jumpGunFrames[f];
            drawW = JUMP_GUN_DRAW_W[f];
            drawH = JUMP_GUN_DRAW_H;
        }
        else
        {
            int f = clampI(assassin.jumpFrame, 0, JUMP_FRAME_COUNT - 1);
            sprite = jumpFrames[f];
        }
        hipOffsetX = drawW / 2;
        return;
    }
    
    if (isKatanaEquipped)
    {
        if (isKatanaAttacking)
        {
            drawW = KATANA_DRAW_W; drawH = KATANA_DRAW_H; yOffset = SPRITE_Y_OFFSET; hipOffsetX = drawW / 2;
            int f = clampI(katanaAttackFrame + 1, 1, 5);
            sprite = katanaAttackFrames[f];
        }
        else if (playerIsMoving())
        {
            drawW = KATANA_RUN_DRAW_W; drawH = KATANA_RUN_DRAW_H; yOffset = KATANA_RUN_Y_OFFSET; hipOffsetX = drawW / 2;
            int f = assassin.walkFrame;
            if (f < 0 || f >= KATANA_RUN_FRAME_COUNT) f = 0;
            sprite = katanaRunFrames[f];
        }
        else
        {
            drawW = KATANA_IDLE_DRAW_W; drawH = KATANA_IDLE_DRAW_H; yOffset = KATANA_IDLE_Y_OFFSET; hipOffsetX = drawW / 2;
            sprite = katanaAttackFrames[0];
        }
        return;
    }

    if (isKnifeEquipped)
    {
        if (isKatanaAttacking)   
        {
            drawW = KNIFE_DRAW_W; drawH = KNIFE_DRAW_H; yOffset = SPRITE_Y_OFFSET; hipOffsetX = drawW / 2;
            int f = clampI(katanaAttackFrame, 0, KNIFE_ATTACK_FRAME_COUNT - 1);
            sprite = assassin.facingRight ? knifeAttackRightFrames[f] : knifeAttackLeftFrames[f];
            spriteIsPreOriented = true;
        }
        else if (playerIsMoving())
        {
            drawW = KNIFE_RUN_DRAW_W; drawH = KNIFE_RUN_DRAW_H; yOffset = KNIFE_RUN_Y_OFFSET; hipOffsetX = drawW / 2;
            int f = assassin.walkFrame;
            if (f < 0 || f >= KNIFE_WALK_FRAME_COUNT) f = 0;
            sprite = assassin.facingRight ? knifeWalkRightFrames[f] : knifeWalkLeftFrames[f];
            spriteIsPreOriented = true;
        }
        else
        {
            drawW = KNIFE_IDLE_DRAW_W; drawH = KNIFE_IDLE_DRAW_H; yOffset = KNIFE_IDLE_Y_OFFSET; hipOffsetX = drawW / 2;
            sprite = knifeIdleTex;   
        }
        return;
    }

    if (isRpgEquipped)
    {
        if (playerIsMoving())
        {
            int f = assassin.walkFrame;
            if (f < 0 || f >= WALK_RPG_FRAME_COUNT) f = 0;
            sprite = walkRpgFrames[f];
            drawW = RPG_WALK_DRAW_W[f]; drawH = RPG_WALK_DRAW_H[f]; yOffset = -RPG_WALK_PAD_BOTTOM[f]; hipOffsetX = RPG_WALK_HIP_OFFSET_X[f];
        }
        else
        {
            sprite = rpgStandTexture;
            drawW = RPG_STAND_DRAW_W; drawH = RPG_STAND_DRAW_H; yOffset = -RPG_STAND_PAD_BOTTOM; hipOffsetX = RPG_STAND_HIP_OFFSET_X;
        }
        return;
    }

    if (gunEquipped)
    {
        if (state == LEVEL3 || state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4) { drawW = L3_SPRITE_DRAW_W; drawH = L3_SPRITE_DRAW_H; yOffset = L3_SPRITE_Y_OFFSET; }
        else { drawW = GUN_SPRITE_DRAW_W; drawH = GUN_SPRITE_DRAW_H; yOffset = GUN_SPRITE_Y_OFFSET; }
        hipOffsetX = drawW / 2; usingGunPose = true;

        if (playerIsMoving()) { int f = assassin.walkFrame; if (f >= WALK_GUN_FRAME_COUNT) f = 0; sprite = walkGunFrames[f]; gunPose = GUN_NEUTRAL; }
        else { gunPose = chooseGunPose(); sprite = gunFrames[gunPose]; }
        return;
    }

    if (playerIsMoving())
    {
        int f = assassin.walkFrame;
        if (f < 0 || f >= WALK_FRAME_COUNT) f = 0;
        sprite = walkRightFrames[f];
    }
    else
    {
        sprite = idleFrame;
        if (state == LEVEL3 || state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4) { drawW = L3_IDLE_DRAW_W; drawH = L3_IDLE_DRAW_H; yOffset = L3_IDLE_Y_OFFSET; }
        else { drawW = IDLE_DRAW_W; drawH = IDLE_DRAW_H; yOffset = IDLE_Y_OFFSET; }
        hipOffsetX = drawW / 2;
    }
}

void drawPlayer()
{
    double screenX = worldToScreen(assassin.x);
    double groundY = groundYAt(assassin.x);
    bool overGapHere = (state != LEVEL3 && state != LEVEL3_INTERIOR && state != LEVEL3_INTERIOR_2 && state != LEVEL3_VAULT && state != LEVEL4) && isOverGap(assassin.x);
    if (!overGapHere)
        drawGroundShadow(screenX, groundY, assassin.y - groundY);

    unsigned int sprite;
    int drawW, drawH, gunPose, hipOffsetX;
    double yOffset;
    bool usingGunPose;
    bool spriteIsPreOriented;

    pickPlayerSprite(sprite, drawW, drawH, yOffset, gunPose, usingGunPose, hipOffsetX, spriteIsPreOriented);
    lastGunPoseIdx = gunPose;

    if (sprite == 0) return;

    // Level 2 has tighter floor spacing than the other scenes.
    if (currentLevel == 2)
    {
        const double level2SpriteScale = 0.55;
        const double level2SpriteYAdjustment = 8.0;
        drawW = (int)(drawW * level2SpriteScale);
        drawH = (int)(drawH * level2SpriteScale);
        yOffset = yOffset * level2SpriteScale + level2SpriteYAdjustment;
        hipOffsetX = (int)(hipOffsetX * level2SpriteScale);
    }

    double recoilOffset = 0.0;
    if (playerRecoilTimer > 0.0)
    {
        double t = playerRecoilTimer / PLAYER_RECOIL_DURATION;
        recoilOffset = PLAYER_RECOIL_PIXELS * t * (assassin.facingRight ? -1.0 : 1.0);
    }

    double drawX = screenX - hipOffsetX + recoilOffset;
    double drawY = assassin.y + yOffset;

    bool flip = spriteIsPreOriented ? false : !assassin.facingRight;
    drawImageFlipped(drawX, drawY, drawW, drawH, sprite, flip, screenX);

    if (usingGunPose && muzzleFlashTimer > 0.0)
    {
        NozzleOffset noz = gunNozzle[gunPose];
        if (currentLevel == 2)
        {
            const double level2SpriteScale = 0.55;
            noz.dx *= level2SpriteScale;
            noz.dy *= level2SpriteScale;
        }
        double flashX = mirrorX(drawX + noz.dx, screenX, !assassin.facingRight);
        double flashY = drawY + noz.dy;
        drawMuzzleFlash(flashX, flashY, muzzleFlashTimer);
    }
}
#endif