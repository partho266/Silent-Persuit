#ifndef WORLD_H
#define WORLD_H

// =========================================================
// SILENT PURSUIT - World.h
// ---------------------------------------------------------
// Level 1 is wider than the window, so every object stores a
// "world X". The camera decides which slice of the world we
// can actually see:
//
//      screenX = worldX - cameraX
//
// This file holds the camera, the shape of the ground, and
// the drawing code for the platform, the rock and the block.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Draw.h"


// ---------------------------------------------------------
// 1. CAMERA
// ---------------------------------------------------------

double cameraX = 0.0;
const double CAMERA_FOLLOW = 0.12;   // 0 = never moves, 1 = snaps instantly


// ---------------------------------------------------------
// 2. SCREEN SHAKE (used by the landmine explosion)
// ---------------------------------------------------------

double shakeTimer = 0.0;
double shakeTotalTime = 0.0;
double shakeStrength = 0.0;
double shakeOffsetX = 0.0;

void startShake(double duration, double strength)
{
    shakeTimer = duration;
    shakeTotalTime = duration;
    shakeStrength = strength;
}

void updateShake()
{
    if (shakeTimer <= 0.0)
    {
        shakeOffsetX = 0.0;
        return;
    }

    countDown(shakeTimer);

    // Wobbles quickly left-right and fades out as time runs down.
    double fade = shakeTimer / shakeTotalTime;
    shakeOffsetX = sin(shakeTimer * 55.0) * shakeStrength * fade;
}


// ---------------------------------------------------------
// 3. WORLD  ->  SCREEN
// ---------------------------------------------------------

double worldToScreen(double worldX)
{
    return worldX - cameraX + shakeOffsetX;
}

// Moves the camera smoothly towards the player, but never
// far enough to show the empty space outside the level.
void followCamera(double targetWorldX)
{
    double cameraTarget = targetWorldX - WIN_W / 2.0;
    cameraTarget = clampD(cameraTarget, 0.0, WORLD_WIDTH - WIN_W);
    cameraX += (cameraTarget - cameraX) * CAMERA_FOLLOW;
}


// ---------------------------------------------------------
// 4. SHAPE OF THE GROUND
// ---------------------------------------------------------

// True when this X is over the broken part of the bridge.
bool isOverGap(double worldX)
{
    return (worldX > GAP_START_X && worldX < GAP_END_X);
}

// Ground height at any X. Scene 1 is lower than scene 2, so
// the two heights are blended over a short stretch, otherwise
// there would be a sudden step in the middle of the map.
double groundYAt(double worldX)
{
    const double blendStart = WIN_W - 40.0;
    const double blendEnd = WIN_W + 40.0;

    if (worldX <= blendStart) return GROUND_Y;
    if (worldX >= blendEnd)   return GROUND_Y_SCENE2;

    double t = (worldX - blendStart) / (blendEnd - blendStart);
    return GROUND_Y + t * (GROUND_Y_SCENE2 - GROUND_Y);
}

// 1 = the outer courtyard, 2 = the castle gates
int sceneAt(double worldX)
{
    return (worldX >= WIN_W) ? 2 : 1;
}


// ---------------------------------------------------------
// 5. DRAWING THE TERRAIN
// ---------------------------------------------------------

void drawWorldPlatform(double worldX0, double worldX1)
{
    int blockW = 42;

    for (double wx = worldX0; wx < worldX1; wx += blockW)
    {
        double segEnd = wx + blockW;
        if (segEnd > worldX1) segEnd = worldX1;

        // Cut the bricks off where the broken bridge starts...
        if (wx < GAP_START_X && segEnd > GAP_START_X) segEnd = GAP_START_X;

        // ...and draw nothing at all inside the gap.
        if (wx >= GAP_START_X && wx < GAP_END_X)
        {
            wx = GAP_END_X - blockW;
            continue;
        }

        double w = segEnd - wx;
        if (w <= 0) continue;

        int y = (int)groundYAt(wx);
        int blockH = 30;

        int sx = (int)worldToScreen(wx);
        int sw = (int)w;

        iSetColor(96, 70, 38);                          // brick body
        iFilledRectangle(sx, y - blockH, sw, blockH);

        iSetColor(150, 118, 62);                        // lit top edge
        iFilledRectangle(sx, y - 10, sw, 10);

        iSetColor(60, 42, 20);                          // seam between bricks
        iLine(sx, y, sx, y - blockH);

        iSetColor(190, 160, 90);                        // rivets
        iFilledCircle(sx + 8, y - 8, 2);
        if (sw > 16) iFilledCircle(sx + sw - 8, y - 8, 2);
    }
}

void drawRockObstacle(double x0, double x1, double groundY, double topY)
{
    int cx = (int)((x0 + x1) / 2.0);
    int baseY = (int)groundY;
    int height = (int)(topY - groundY);
    int width = (int)(x1 - x0);

    // --- soft shadow on the floor ---
    beginTransparency();
    setColorAlpha(0, 0, 0, 0.30);
    iFilledEllipse(cx, baseY + 2, width * 0.55, 6);
    endTransparency();

    // --- stacked stone lumps ---
    iSetColor(95, 92, 88);
    iFilledCircle((int)x0 + width / 6, baseY + height / 3, (int)(width * 0.24));
    iFilledCircle((int)x0 + width / 2, baseY + height / 3, (int)(width * 0.26));
    iFilledCircle((int)x0 + (5 * width) / 6, baseY + height / 3, (int)(width * 0.24));

    iSetColor(120, 116, 110);
    iFilledCircle((int)x0 + width / 3, baseY + height / 2, (int)(width * 0.24));
    iFilledCircle((int)x0 + (2 * width) / 3, baseY + height / 2, (int)(width * 0.24));

    iSetColor(70, 67, 64);
    iFilledCircle((int)x0 + width / 6, baseY + height / 5, (int)(width * 0.14));
    iFilledCircle((int)x0 + width / 2, baseY + height / 6, (int)(width * 0.12));

    // --- flat plateau on top, so the player can land on it ---
    double plateauY0 = baseY + height * 0.62;
    double plateauY1 = topY;
    double plateauX0 = x0 + OBSTACLE_PLATEAU_MARGIN * 0.4;
    double plateauX1 = x1 - OBSTACLE_PLATEAU_MARGIN * 0.4;

    double px[8] =
    {
        plateauX0 + 10, plateauX1 - 10,
        plateauX1,      plateauX1,
        plateauX1 - 10, plateauX0 + 10,
        plateauX0,      plateauX0
    };
    double py[8] =
    {
        plateauY1,      plateauY1,
        plateauY1 - 10, plateauY0 + 14,
        plateauY0,      plateauY0,
        plateauY0 + 14, plateauY1 - 10
    };

    iSetColor(128, 124, 116);
    iFilledPolygon(px, py, 8);

    iSetColor(160, 155, 144);
    iFilledRectangle((int)plateauX0 + 6, (int)plateauY1 - 6, (int)(plateauX1 - plateauX0) - 12, 6);

    iSetColor(75, 72, 68);
    iLine((int)plateauX0, (int)plateauY0, (int)plateauX1, (int)plateauY0);

    // --- small pebbles at the base ---
    iSetColor(80, 76, 72);
    iFilledCircle((int)x0 - 8, baseY + 4, 6);
    iFilledCircle((int)x1 + 10, baseY + 3, 5);
    iFilledCircle(cx, baseY + 2, 4);
}

// The white concrete block that Enemy 1 sits on.
void drawEnemyBlock()
{
    int sx = (int)worldToScreen(ENEMY_BLOCK_X);
    int gy = (int)groundYAt(ENEMY_BLOCK_X);
    int bw = (int)ENEMY_BLOCK_W;
    int bh = (int)ENEMY_BLOCK_H;

    iSetColor(190, 185, 175);
    iFilledRectangle(sx, gy, bw, bh);

    iSetColor(220, 215, 205);                 // lit top face
    iFilledRectangle(sx, gy + bh - 6, bw, 6);

    iSetColor(140, 135, 125);                 // shaded right face
    iFilledRectangle(sx + bw - 8, gy, 8, bh);

    iSetColor(100, 95, 85);                   // outline
    iLine(sx, gy, sx, gy + bh);
    iLine(sx, gy + bh, sx + bw, gy + bh);
}

#endif
