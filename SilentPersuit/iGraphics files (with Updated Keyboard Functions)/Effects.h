#ifndef EFFECTS_H
#define EFFECTS_H

// =========================================================
// SILENT PURSUIT - Effects.h
// ---------------------------------------------------------
// Pure visual effects. Nothing in this file changes the game
// rules, it only draws things: the muzzle flash, the bullet
// hit mark, the shadow under a character and the landmine
// explosion.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Draw.h"


// ---------------------------------------------------------
// 1. MUZZLE FLASH
// ---------------------------------------------------------

const double MUZZLE_FLASH_DURATION = 0.06;

void drawMuzzleFlash(double x, double y, double flashTimer)
{
    if (flashTimer <= 0.0) return;

    double t = clampD(flashTimer / MUZZLE_FLASH_DURATION, 0.0, 1.0);

    beginTransparency();

    setColorAlpha(255, 217, 76, 0.55 * t);              // warm outer glow
    iFilledEllipse(x, y, 16.0 * t + 4.0, (16.0 * t + 4.0) * 0.8);

    setColorAlpha(255, 255, 230, 0.90 * t);             // bright core
    iFilledCircle(x, y, 7.0 * t + 2.0);

    endTransparency();

    iSetColor(255, 240, 180);                           // cross-shaped sparks
    double spikeLen = 14.0 * t + 3.0;
    iLine((int)(x - spikeLen), (int)y, (int)(x + spikeLen), (int)y);
    iLine((int)x, (int)(y - spikeLen * 0.6), (int)x, (int)(y + spikeLen * 0.6));
}


// ---------------------------------------------------------
// 2. BULLET HIT MARKER
// ---------------------------------------------------------
// A short "X" that pops up exactly where the bullet landed.
// Yellow = headshot, red = body shot. It tells the player
// which part of the enemy was actually hit.
// ---------------------------------------------------------

const double HIT_MARK_DURATION = 0.35;

double hitMarkTimer = 0.0;
double hitMarkX = 0.0;
double hitMarkY = 0.0;
bool   hitMarkWasHead = false;

void showHitMarker(double screenX, double screenY, bool headshot)
{
    hitMarkTimer = HIT_MARK_DURATION;
    hitMarkX = screenX;
    hitMarkY = screenY;
    hitMarkWasHead = headshot;
}

void drawHitMarker()
{
    if (hitMarkTimer <= 0.0) return;

    double t = hitMarkTimer / HIT_MARK_DURATION;    // 1.0 -> 0.0 as it fades
    double outer = 8.0 + (1.0 - t) * 10.0;          // the X grows while fading
    double inner = outer * 0.35;

    beginTransparency();

    if (hitMarkWasHead) setColorAlpha(255, 230, 50, t);
    else                setColorAlpha(255, 75, 50, t);

    setLineThickness(hitMarkWasHead ? 3.0 : 2.0);

    // four short strokes making an "X" with a gap in the middle
    iLine(hitMarkX - outer, hitMarkY - outer, hitMarkX - inner, hitMarkY - inner);
    iLine(hitMarkX + inner, hitMarkY + inner, hitMarkX + outer, hitMarkY + outer);
    iLine(hitMarkX - outer, hitMarkY + outer, hitMarkX - inner, hitMarkY + inner);
    iLine(hitMarkX + inner, hitMarkY - inner, hitMarkX + outer, hitMarkY - outer);

    setLineThickness(1.0);
    endTransparency();
}


// ---------------------------------------------------------
// 3. GROUND SHADOW UNDER A CHARACTER
// ---------------------------------------------------------
// The higher the character is, the smaller and fainter the
// shadow becomes.
// ---------------------------------------------------------

void drawGroundShadow(double screenX, double groundY, double heightAboveGround)
{
    double scale = clampD(1.0 - (heightAboveGround / 160.0), 0.25, 1.0);

    beginTransparency();
    setColorAlpha(0, 0, 0, 0.35 * scale + 0.15);
    iFilledEllipse(screenX, groundY + 3, 50 * scale, 12 * scale);
    endTransparency();
}


// ---------------------------------------------------------
// 4. LANDMINE EXPLOSION
// ---------------------------------------------------------
// The timer counts DOWN, so through the whole animation:
//
//      t    goes 1.0  ->  0.0     (how much time is left)
//      grow goes 0.0  ->  1.0     (how far it has spread)
//
// Anything that expands or rises uses grow, anything that
// fades away uses t.
// ---------------------------------------------------------

const double EXPLOSION_DURATION = 1.5;
const int    DEBRIS_COUNT = 14;

void drawExplosion(double screenX, double screenY, double timer)
{
    if (timer <= 0.0) return;

    double t = timer / EXPLOSION_DURATION;
    double grow = 1.0 - t;

    beginTransparency();

    // --- shockwave ring: only at the very start of the blast ---
    if (grow < 0.35)
    {
        double ringT = grow / 0.35;                 // 0.0 -> 1.0
        double ringR = 20.0 + ringT * 190.0;

        setColorAlpha(255, 242, 190, 0.55 * (1.0 - ringT));
        setLineThickness(3.0);
        iEllipse(screenX, screenY + 20, ringR, ringR * 0.55);
        setLineThickness(1.0);
    }

    // --- expanding smoke cloud ---
    setColorAlpha(51, 51, 51, 0.80 * t);
    iFilledCircle(screenX, screenY + grow * 120, 50 + grow * 80);
    iFilledCircle(screenX - 40 - grow * 40, screenY + grow * 90 + 20, 40 + grow * 60);
    iFilledCircle(screenX + 40 + grow * 40, screenY + grow * 100 + 10, 45 + grow * 65);

    // --- dust rolling outwards along the ground ---
    setColorAlpha(26, 26, 26, 0.90 * t);
    iFilledCircle(screenX - 70 * grow, screenY + 40 * grow, 20 + grow * 20);
    iFilledCircle(screenX + 60 * grow, screenY + 50 * grow, 25 + grow * 25);

    // --- flying debris ---
    // Every piece is thrown out at its own fixed angle and is
    // pulled back down, so the pattern looks the same each time.
    for (int i = 0; i < DEBRIS_COUNT; i++)
    {
        double angle = M_PI * (0.12 + 0.76 * (i / (double)(DEBRIS_COUNT - 1)));
        double speed = 210.0 + (i % 5) * 45.0;

        double px = screenX + cos(angle) * speed * grow;
        double py = screenY + 10 + sin(angle) * speed * grow - 300.0 * grow * grow;

        if (py < screenY - 10) continue;   // this piece already landed

        setColorAlpha(64, 56, 51, t);
        iFilledCircle(px, py, 3 + (i % 3));
    }

    // --- fire burst: only in the first half of the animation ---
    if (t > 0.5)
    {
        double fireT = (t - 0.5) / 0.5;    // 1.0 -> 0.0
        double rise = 1.0 - fireT;
        double pulse = 1.0 + 0.16 * sin(animTime * 42.0);
        double flicker = 0.86 + 0.14 * sin(animTime * 67.0 + 1.3);

        setColorAlpha(255, 90, 0, 0.90 * fireT * flicker);               // orange
        iFilledCircle(screenX, screenY + rise * 40 + 10, (40 + rise * 50) * pulse);

        setColorAlpha(255, 205, 35, 1.00 * fireT * flicker);              // yellow
        iFilledCircle(screenX, screenY + rise * 20 + 10, (25 + rise * 30) * pulse);

        setColorAlpha(255, 255, 225, 1.00 * fireT);                       // white core
        iFilledCircle(screenX, screenY + rise * 10 + 10, (10 + rise * 15) * pulse);

        // Animated tongues of flame give the fire a changing silhouette.
        setColorAlpha(255, 145, 20, 0.82 * fireT);
        for (int i = 0; i < 5; i++)
        {
            double phase = animTime * (28.0 + i * 3.0) + i * 1.7;
            double tongueX = screenX + sin(phase) * (8.0 + i * 5.0);
            double tongueY = screenY + 28.0 + rise * (35.0 + i * 9.0) + sin(phase * 0.7) * 8.0;
            iFilledEllipse(tongueX, tongueY, 8.0 + i * 2.0, 22.0 + i * 4.0);
        }
    }

    endTransparency();
}


// ---------------------------------------------------------
// 5. WHITE FLASH OVER THE WHOLE SCREEN
// ---------------------------------------------------------

void drawScreenFlash(double timer, double duration)
{
    if (timer <= 0.0) return;

    double t = timer / duration;

    beginTransparency();
    setColorAlpha(255, 242, 204, 0.55 * t);
    iFilledRectangle(0, 0, WIN_W, WIN_H);
    endTransparency();
}

#endif
