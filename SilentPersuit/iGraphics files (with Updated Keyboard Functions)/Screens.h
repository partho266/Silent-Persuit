#ifndef SCREENS_H
#define SCREENS_H

#include "Config.h"
#include "Draw.h"
#include "Assets.h"
#include "World.h"
#include "Effects.h"
#include "UI.h"
#include "Player.h"
#include "Enemy.h"
#include "Landmine.h"
#include "Level1.h"
#include "Level3Intro.h"
#include "Level3.h"
#include "Level4.h"

#include <stdio.h>
#include <stdlib.h>


int loadingProgress = 0;
bool loadingDone = false;

void drawWoodenAtriumCeiling();

void drawLoadingScreen()
{
    iShowImage(0, 0, WIN_W, WIN_H, loadingBgTexture);
    int barY = 60, barW = 500, barH = 24, startX = WIN_W / 2 - barW / 2;

    double bgX[6] = { startX - 25.0, startX * 1.0, startX + barW * 1.0, startX + barW + 25.0, startX + barW * 1.0, startX * 1.0 };
    double bgY[6] = { barY + barH / 2.0, barY + barH + 6.0, barY + barH + 6.0, barY + barH / 2.0, barY - 6.0, barY - 6.0 };

    iSetColor(110, 110, 115); iFilledPolygon(bgX, bgY, 6);
    iSetColor(180, 180, 190); iLine(bgX[0], bgY[0], bgX[1], bgY[1]); iLine(bgX[1], bgY[1], bgX[2], bgY[2]);
    iSetColor(130, 130, 140); iLine(bgX[2], bgY[2], bgX[3], bgY[3]);
    iSetColor(35, 35, 40); iLine(bgX[0], bgY[0], bgX[5], bgY[5]); iLine(bgX[5], bgY[5], bgX[4], bgY[4]); iLine(bgX[4], bgY[4], bgX[3], bgY[3]);

    iSetColor(40, 40, 40); iFilledRectangle(startX, barY, barW, barH);
    iSetColor(20, 20, 20); iLine(startX, barY + barH, startX + barW, barY + barH);
    iSetColor(90, 90, 90); iLine(startX, barY, startX + barW, barY);

    int fillW = barW * loadingProgress / 100;
    if (fillW > 0)
    {
        iSetColor(25, 130, 190); iFilledRectangle(startX, barY + barH / 2, fillW, barH / 2);
        iSetColor(0, 90, 140); iFilledRectangle(startX, barY, fillW, barH / 2);
        iSetColor(100, 190, 240); iLine(startX, barY + barH - 1, startX + fillW, barY + barH - 1);
    }

    char buf[20]; sprintf(buf, "%d%%", loadingProgress);
    if (fillW > 50) { iSetColor(255, 255, 255); iText(startX + fillW - 45, barY + 6, buf, GLUT_BITMAP_HELVETICA_18); }
    else { iSetColor(200, 200, 200); iText(startX + fillW + 10, barY + 6, buf, GLUT_BITMAP_HELVETICA_18); }
}

void drawMenuScreen() { iShowImage(0, 0, WIN_W, WIN_H, menuBgTexture); iSetColor(255, 255, 255); iText(WIN_W / 2 - 90, 500, "SILENT PURSUIT", GLUT_BITMAP_TIMES_ROMAN_24); drawButton(btnNewGame); drawButton(btnOptions); drawButton(btnCredits); drawButton(btnQuit); }
void drawLevelSelectScreen() { iShowImage(0, 0, WIN_W, WIN_H, menuBgTexture); iSetColor(255, 255, 255); iText(WIN_W / 2 - 80, 500, "SELECT LEVEL", GLUT_BITMAP_TIMES_ROMAN_24); drawButton(btnLevel1); drawButton(btnLevel2); drawButton(btnLevel3); drawButton(btnLevel4); drawButton(btnLvlBack); }
void drawOptionsScreen() { iSetColor(14, 11, 8); iFilledRectangle(0, 0, WIN_W, WIN_H); iSetColor(255, 255, 255); iText(WIN_W / 2 - 40, 500, "OPTIONS", GLUT_BITMAP_TIMES_ROMAN_24); drawButton(btnAudio); drawButton(btnControls); drawButton(btnOptBack); }
void drawCreditsScreen() { iSetColor(14, 11, 8); iFilledRectangle(0, 0, WIN_W, WIN_H); iSetColor(255, 255, 255); iText(WIN_W / 2 - 50, 500, "CREDITS", GLUT_BITMAP_TIMES_ROMAN_24); drawButton(btnCredBack); }

void drawGameplayHud()
{
    iSetColor(255, 255, 255);
    char hpBuf[32]; sprintf(hpBuf, "HP: %d / %d", playerHP, PLAYER_MAX_HP);
    iText(15, WIN_H - 90, hpBuf, GLUT_BITMAP_HELVETICA_18);
    iText(15, WIN_H - 50, "A/D to move | W to jump | G to draw/holster gun | Left-click to fire", GLUT_BITMAP_HELVETICA_12);
}

void drawGameplayScreen()
{
    iShowImage((int)worldToScreen(0), 0, WIN_W, WIN_H, level1BgTexture);
    iShowImage((int)worldToScreen(WIN_W), 0, WIN_W, WIN_H, level1Scene3BgTexture);
    drawWorldPlatform(0.0, GAP_START_X);
    drawWorldPlatform(GAP_END_X, WORLD_WIDTH);
    drawLandmine();
    drawEnemyBlock();
    drawRockObstacle(worldToScreen(OBSTACLE_X0), worldToScreen(OBSTACLE_X1), GROUND_Y_SCENE2, OBSTACLE_TOP_Y);
    drawEnemy(enemy1);
    drawEnemy(enemy2);
    bool showingBombDeath = l3TearSequenceActive || l3DeathMessageActive;
    if (!showingBombDeath)
        drawPlayer();
    drawLandmineBlast();
    drawHitMarker();
    drawCrosshair();
    drawScreenFlash(blastFlashTimer, BLAST_FLASH_DURATION);
    drawGameplayHud();

    if (level1Complete)
    {
        beginTransparency();
        setColorAlpha(0, 0, 0, 0.62);
        iFilledRectangle(0, 150, WIN_W, 260);
        endTransparency();

        iSetColor(255, 215, 80);
        iText(WIN_W / 2 - 190, 330, "LEVEL 1 COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 245, 190);
        iText(WIN_W / 2 - 245, 285, "THE LANDMINE HAS BEEN DESTROYED", GLUT_BITMAP_HELVETICA_18);
        iSetColor(230, 210, 175);
        iText(WIN_W / 2 - 180, 245, "Mission accomplished.", GLUT_BITMAP_HELVETICA_18);
    }
}

void drawLevel3IntroScreen()
{
    if (level3IntroShot >= LEVEL3_INTRO_SHOT_COUNT) return;

    double t = level3IntroTimer;
    double zoomProgress = clampD(t / L3_SHOT_TIME, 0.0, 1.0);
    double zoom = L3_ZOOM_START + (L3_ZOOM_END - L3_ZOOM_START) * zoomProgress;

    int drawW = (int)(WIN_W * zoom);
    int drawH = (int)(WIN_H * zoom);
    int drawX = (WIN_W - drawW) / 2;
    int drawY = (WIN_H - drawH) / 2;

    iShowImage(drawX, drawY, drawW, drawH, level3IntroTexture[level3IntroShot]);

    const int barH = 46;
    iSetColor(0, 0, 0); iFilledRectangle(0, WIN_H - barH, WIN_W, barH); iFilledRectangle(0, 0, WIN_W, barH);

    double curtainAlpha = 0.0;
    if (t < L3_FADE_IN_TIME) curtainAlpha = 1.0 - (t / L3_FADE_IN_TIME);
    else if (t > L3_SHOT_TIME - L3_FADE_OUT_TIME) curtainAlpha = (t - (L3_SHOT_TIME - L3_FADE_OUT_TIME)) / L3_FADE_OUT_TIME;

    if (curtainAlpha > 0.0) { beginTransparency(); setColorAlpha(0, 0, 0, curtainAlpha); iFilledRectangle(0, 0, WIN_W, WIN_H); endTransparency(); }

    iSetColor(190, 190, 190); iText(WIN_W - 150, 16, "Click to skip", GLUT_BITMAP_HELVETICA_12);
}

void drawLevel3Screen()
{
    int shakeX = 0, shakeY = 0;
    if (isRpgFired && blastState < 2) { shakeX = (rand() % 17) - 8; shakeY = (rand() % 17) - 8; }

    if (!isRpgFired) iShowImage(shakeX, shakeY, WIN_W, WIN_H, level3StartBgTexture);
    else
    {
        if (blastState == 0) iShowImage(shakeX, shakeY, WIN_W, WIN_H, level3BlastTexture[0]);
        else if (blastState == 1) iShowImage(shakeX, shakeY, WIN_W, WIN_H, level3BlastTexture[1]);
        else
        {
            iShowImage(0, 0, WIN_W, WIN_H, level3AftermathTexture);
            beginTransparency();
            for (int i = 0; i < MAX_DUST_PARTICLES; i++) if (dust[i].active) { setColorAlpha(160, 140, 120, dust[i].alpha); iFilledCircle((int)dust[i].x, (int)dust[i].y, (int)dust[i].radius); }
            endTransparency();
        }

    if (l3WarningFlashTimer > 0.0)
    {
        double flashAlpha = clampD(l3WarningFlashTimer / 0.45, 0.0, 1.0) * 0.65;
        beginTransparency();
        setColorAlpha(255, 0, 0, flashAlpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }
    }

    if (level3GameplayActive)
    {
        drawPlayer();
        iSetColor(210, 205, 195); iText(WIN_W - 200, 30, "Walk Right To Enter --->", GLUT_BITMAP_HELVETICA_18);
        return;
    }

    const int charCenterX = (WIN_W / 2 - 10) + shakeX;
    const int groundY = 60 + shakeY;

    if (level3Phase == L3_START) drawImageFlipped(charCenterX - RPG_PROP_DRAW_W / 2 - 40, groundY - 8, RPG_PROP_DRAW_W, RPG_PROP_DRAW_H, rpgPropTexture, false, charCenterX);

    if (level3Phase == L3_START) drawImageFlipped(charCenterX - IDLE_DRAW_W / 2, groundY, IDLE_DRAW_W, IDLE_DRAW_H, idleFrame, false, charCenterX);
    else if (level3Phase == L3_READY) drawImageFlipped(charCenterX - RPG_STAND_HIP_OFFSET_X, groundY - RPG_STAND_PAD_BOTTOM, RPG_STAND_DRAW_W, RPG_STAND_DRAW_H, rpgStandTexture, false, charCenterX);
    else { int f = level3Phase - L3_LIFT_1; drawImageFlipped(charCenterX - RPG_LIFT_HIP_OFFSET_X[f], groundY - RPG_LIFT_PAD_BOTTOM[f], RPG_LIFT_DRAW_W[f], RPG_LIFT_DRAW_H[f], rpgLiftFrames[f], false, charCenterX); }

    if (level3Phase == L3_READY && !isRpgFired) { iSetColor(230, 210, 175); iText(WIN_W / 2 - 105 + shakeX, 40 + shakeY, "RPG equipped - Left click to fire!", GLUT_BITMAP_HELVETICA_12); }
    if (isRpgFired && blastState == 0)
    {
        int nozzleX = charCenterX + 95, nozzleY = groundY + 255;
        iSetColor(255, 100, 0);   iFilledCircle(nozzleX, nozzleY, 35);
        iSetColor(255, 220, 50);  iFilledCircle(nozzleX, nozzleY, 20);
        iSetColor(255, 255, 255); iFilledCircle(nozzleX, nozzleY, 8);
    }
}

void drawLevel3InteriorScreen()
{
    bool alarmActive = securityShakeTimer > 0.0;
    double shakeX = 0.0, shakeY = 0.0;
    if (alarmActive)
    {
        shakeX = ((rand() % 100) / 100.0 - 0.5) * 2.0 * SECURITY_SHAKE_STRENGTH;
        shakeY = ((rand() % 100) / 100.0 - 0.5) * 2.0 * SECURITY_SHAKE_STRENGTH;
    }

    // Everything below is nudged by the same jitter each frame so the
    // whole scene shakes together, like the room itself is rattling.
    glPushMatrix();
    // Include the bomb's camera shake in the whole chamber, not only in
    // the explosion position.
    glTranslated(shakeX + shakeOffsetX, shakeY, 0.0);

    iShowImage(0, 0, WIN_W, WIN_H, level3InteriorTexture);

    drawWoodenAtriumCeiling();

    for (int i = 0; i < MAX_NINJAS; i++)
    {
        if (ninjas[i].active)
        {
            double drawX = ninjas[i].x - NINJA_DRAW_W / 2.0;

            if (ninjas[i].state == NINJA_DEAD)
            {
                double deadY = ninjas[i].y + NINJA_DEAD_Y_OFFSET;
                drawImageFlipped(drawX, deadY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaDeadTex, true, ninjas[i].x);
            }
            else
            {
                double aliveY = ninjas[i].y + NINJA_Y_OFFSET;

                unsigned int ninjaTex = 0;
                if (ninjas[i].state == NINJA_RUNNING) ninjaTex = ninjaRunFrames[ninjas[i].animFrame];
                else if (ninjas[i].state == NINJA_ATTACKING) ninjaTex = ninjaAttackFrames[ninjas[i].animFrame];

                bool flipNinja = (ninjas[i].x < assassin.x);
                drawImageFlipped(drawX, aliveY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaTex, flipNinja, ninjas[i].x);
                drawEnemyHpBar(ninjas[i].x, aliveY + NINJA_DRAW_H, ninjas[i].hp, NINJA_MAX_HP);
            }
        }
    }

    bool showingBombDeath = l3TearSequenceActive || l3DeathMessageActive;
    if (!showingBombDeath)
        drawPlayer();

    // Bomb explosion visuals are drawn after characters so the smoke
    // and fire cover them (like landmine explosion does).
    if (l3BombTriggered && l3BombExplTimer > 0.0)
    {
        // Place the time-bomb explosion in the middle of the chamber and
        // scale it up independently of the landmine explosion.
        double bombScreenX = WIN_W / 2.0;
        double bombScreenY = WIN_H / 2.0;
        glPushMatrix();
        glTranslated(bombScreenX, bombScreenY, 0.0);
        glScaled(1.65, 1.65, 1.0);
        glTranslated(-bombScreenX, -bombScreenY, 0.0);
        drawExplosion(bombScreenX, bombScreenY, l3BombExplTimer);
        glPopMatrix();
    }

    // Global white flash for the bomb
    if (l3BombTriggered && l3BombFlashTimer > 0.0)
    {
        drawScreenFlash(l3BombFlashTimer, L3_BOMB_FLASH_DURATION);
    }

    if (l3TearSequenceActive || l3DeathMessageActive)
    {
        const int tornW = 180;
        const int tornH = 270;
        const int tornX = (int)worldToScreen(assassin.x) - tornW / 2;
        const int tornY = (int)groundYAt(assassin.x);
        iShowImage(tornX, tornY, tornW, tornH, tornPlayerTextures[l3TearFrame]);
    }

    glPopMatrix();

    // Pulsing red alarm overlay - fades out as the timer runs down,
    // and flickers a little like a warning light rather than sitting flat.
    if (alarmActive)
    {
        double pulse = 0.5 + 0.5 * sin(securityShakeTimer * 10.0);
        double fade = clampD(securityShakeTimer / 1.0, 0.0, 1.0); // fades out over the last second
        double alarmAlpha = (0.08 + 0.12 * pulse) * fade;

        beginTransparency();
        setColorAlpha(200, 0, 0, alarmAlpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }

    bool allDead = true;
    int deadCount = 0;
    for (int i = 0; i < MAX_NINJAS; i++)
    {
        if (ninjas[i].active && ninjas[i].state != NINJA_DEAD) allDead = false;
        if (ninjas[i].state == NINJA_DEAD) deadCount++;
    }
    if (l3BombActive)
    {
        int secs = (int)ceil(l3BombTimer);
        if (secs < 0) secs = 0;
        if (l3BombTimer <= 2.0) iSetColor(255, 40, 40); else iSetColor(240, 180, 30);
        char buf[64]; sprintf(buf, "TIME BOMB! Escape in %ds", secs);
        iText(WIN_W - 380, 30, buf, GLUT_BITMAP_HELVETICA_18);
    }
    else if (l3BombTriggered)
    {
        iSetColor(255, 20, 20);
        iText(WIN_W - 320, 30, "!! EXPLODING !!", GLUT_BITMAP_HELVETICA_18);
    }
    else if (allDead && currentNinjaSpawnIndex >= MAX_NINJAS)
    {
        iSetColor(210, 205, 195);
        iText(WIN_W - 220, 30, "Clear! Walk Right --->", GLUT_BITMAP_HELVETICA_18);
    }

    if (l3DeathMessageActive)
    {
        beginTransparency();
        setColorAlpha(0, 0, 0, 0.58);
        iFilledRectangle(0, 205, WIN_W, 125);
        endTransparency();

        iSetColor(245, 210, 150);
        iText(155, 270, "Sometimes time doesn't kill you,", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 95, 55);
        iText(265, 235, "timebomb does!", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    drawGameplayHud();
    iSetColor(210, 205, 195);
    iText(15, 30, "A/D to run | LEFT CLICK to swing Katana", GLUT_BITMAP_HELVETICA_12);
}

void drawLevel3TransitionScreen()
{
    if (l3TransitionTimer < 1.0)
    {
        drawLevel3InteriorScreen();

        double alpha = l3TransitionTimer / 1.0;
        beginTransparency();
        setColorAlpha(0, 0, 0, alpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }
    else
    {
        iShowImage(0, 0, WIN_W, WIN_H, level3Interior2Texture);

        double alpha = 1.0 - ((l3TransitionTimer - 1.0) / 1.0);
        beginTransparency();
        setColorAlpha(0, 0, 0, alpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }
}

void drawLevel3TigerScreen()
{
    iShowImage(0, 0, WIN_W, WIN_H, level3TigerAtriumTexture);

    int tigerW = 260;
    int tigerH = 145;
    int tigerY = (int)GROUND_Y - 10;
    unsigned int tigerTexture = l3TigerAttacking ? l3TigerAttackTexture : l3TigerRunFrames[l3TigerAnimFrame];
    // Tiger artwork faces right by default; flip it on the Y axis while it
    // approaches from the right so it faces the player.
    bool tigerFacesLeft = l3TigerX > assassin.x;
    drawImageFlipped((int)l3TigerX - tigerW / 2, tigerY, tigerW, tigerH,
                     tigerTexture, tigerFacesLeft, (int)l3TigerX);

    drawPlayer();

    if (l3TigerFlashTimer > 0.0)
    {
        double alpha = clampD(l3TigerFlashTimer / 0.18, 0.0, 1.0);
        beginTransparency();
        setColorAlpha(255, 0, 0, alpha * 0.72);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }

    drawGameplayHud();
    iSetColor(255, 70, 50);
    iText(WIN_W / 2 - 180, WIN_H - 45, "THE TIGER HAS FOUND YOU!", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawLevel3Interior2Screen()
{
    if (l3DoorChoiceActive)
    {
        iShowImage(0, 0, WIN_W, WIN_H, level3DoorChoiceTexture);

        // Mud-colored ground covering the lower stone floor of 3_8_1.
        // This is drawn only for the two-door scene.
        double mudX[12] = { 0.0, 90.0, 180.0, 270.0, 390.0, 500.0,
                            620.0, 740.0, 860.0, 980.0, 1120.0, (double)WIN_W };
        double mudY[12] = { 74.0, 105.0, 132.0, 130.0, 117.0, 123.0,
                            112.0, 126.0, 119.0, 135.0, 121.0, 130.0 };
        double mudPolyX[24];
        double mudPolyY[24];
        for (int i = 0; i < 12; i++)
        {
            mudPolyX[i] = mudX[i];
            mudPolyY[i] = mudY[i];
            mudPolyX[23 - i] = mudX[i];
            mudPolyY[23 - i] = 0.0;
        }
        iSetColor(78, 55, 37);
        iFilledPolygon(mudPolyX, mudPolyY, 24);

        iSetColor(111, 78, 48);
        iLine(0, (int)mudY[0], WIN_W, (int)mudY[11]);

        // High-contrast option boxes placed in the middle of each door.
        int boxY = WIN_H / 2 - 45;
        int boxW = 240;
        int boxH = 90;
        int leftBoxX = (int)L3_DOOR_LEFT_X - boxW / 2;
        int rightBoxX = (int)L3_DOOR_RIGHT_X - boxW / 2;

        beginTransparency();
        setColorAlpha(20, 12, 8, 0.82);
        iFilledRectangle(leftBoxX, boxY, boxW, boxH);
        iFilledRectangle(rightBoxX, boxY, boxW, boxH);
        endTransparency();

        iSetColor(218, 165, 78);
        iRectangle(leftBoxX, boxY, boxW, boxH);
        iRectangle(rightBoxX, boxY, boxW, boxH);

        iSetColor(255, 220, 130);
        iText(leftBoxX + 78, boxY + 57, "DOOR 1", GLUT_BITMAP_HELVETICA_18);
        iText(rightBoxX + 78, boxY + 57, "DOOR 2", GLUT_BITMAP_HELVETICA_18);

        if (hasSecurityKey)
        {
            iSetColor(255, 255, 255);
            if (fabs(assassin.x - L3_DOOR_LEFT_X) <= 130.0)
                iText(leftBoxX + 35, boxY + 25, "Press K: Enter Key", GLUT_BITMAP_HELVETICA_12);
            if (fabs(assassin.x - L3_DOOR_RIGHT_X) <= 130.0)
                iText(rightBoxX + 35, boxY + 25, "Press K: Enter Key", GLUT_BITMAP_HELVETICA_12);
        }
        else
        {
            iSetColor(255, 210, 80);
            iText(WIN_W / 2 - 120, boxY - 35, "You need the guard's key", GLUT_BITMAP_HELVETICA_18);
        }

        if (l3WrongDoorMessage)
        {
            iSetColor(255, 70, 50);
            iText(WIN_W / 2 - 170, WIN_H - 80, "Wrong door! Something terrible awaits...", GLUT_BITMAP_HELVETICA_18);
        }

        drawPlayer();
        drawGameplayHud();
        return;
    }

    iShowImage(0, 0, WIN_W, WIN_H, level3Interior2Texture);

    drawWoodenAtriumCeiling();

    // Two usable hooks hanging below the wooden ceiling.
    for (int i = 0; i < 2; i++)
    {
        int hx = (int)CEILING_HOOK_X[i];
        iSetColor(45, 25, 12);
        iFilledRectangle(hx - 3, (int)CEILING_HOOK_Y, 6, 30);
        iSetColor(135, 83, 38);
        iFilledCircle(hx, (int)CEILING_HOOK_Y - 2, 10);
        iSetColor(210, 145, 62);
        iCircle(hx, (int)CEILING_HOOK_Y - 2, 6);
    }

    if (ceilingHangState == CEILING_GROUND &&
        (fabs(assassin.x - CEILING_HOOK_X[0]) <= CEILING_HANG_RANGE ||
         fabs(assassin.x - CEILING_HOOK_X[1]) <= CEILING_HANG_RANGE))
    {
        iSetColor(255, 220, 130);
        iText((int)assassin.x - 55, (int)assassin.y + 45, "Press E to hang", GLUT_BITMAP_HELVETICA_12);
    }

    if (isKeyDropped)
    {
        iSetColor(255, 220, 0);
        iFilledCircle(keyX, keyY + 8, 6);
        iFilledRectangle(keyX + 3, keyY + 6, 20, 4);
        iFilledRectangle(keyX + 15, keyY + 2, 4, 4);
        iFilledRectangle(keyX + 20, keyY + 2, 4, 4);

        if (fabs(assassin.x - keyX) < 60.0)
        {
            iSetColor(255, 255, 255);
            iText((int)keyX - 60, (int)keyY + 40, "Press 'K' to pick up Key", GLUT_BITMAP_HELVETICA_18);
        }
    }

    if (ambushTriggered)
    {
        for (int i = 0; i < MAX_NINJAS; i++)
        {
            if (ninjas[i].active)
            {
                double drawX = ninjas[i].x - NINJA_DRAW_W / 2.0;

                if (ninjas[i].state == NINJA_DEAD)
                {
                    double deadY = ninjas[i].y + NINJA_DEAD_Y_OFFSET;
                    drawImageFlipped(drawX, deadY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaDeadTex, true, ninjas[i].x);
                }
                else
                {
                    double aliveY = ninjas[i].y + NINJA_Y_OFFSET;

                    unsigned int ninjaTex = 0;
                    if (ninjas[i].state == NINJA_RUNNING)
                    {
                        ninjaTex = (ninjas[i].type == RED_NINJA) ? redNinjaRunFrames[ninjas[i].animFrame] : ninjaRunFrames[ninjas[i].animFrame];
                    }
                    else if (ninjas[i].state == NINJA_ATTACKING)
                    {
                        ninjaTex = (ninjas[i].type == RED_NINJA) ? redNinjaAttackFrames[ninjas[i].animFrame] : ninjaAttackFrames[ninjas[i].animFrame];
                    }

                    bool flipNinja = (ninjas[i].x < assassin.x);
                    drawImageFlipped(drawX, aliveY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaTex, flipNinja, ninjas[i].x);
                    drawEnemyHpBar(ninjas[i].x, aliveY + NINJA_DRAW_H, ninjas[i].hp, NINJA_MAX_HP);
                }
            }
        }
    }

    for (int i = 0; i < L3_SPIDER_COUNT; i++)
    {
        if (!l3SpiderActive[i]) continue;
        double spiderDrawX = l3SpiderX[i] - LIBRARY_SPIDER_DRAW_W / 2.0;
        double spiderDrawY = GROUND_Y + LIBRARY_SPIDER_Y_OFFSET;
        drawImageFlipped(spiderDrawX, spiderDrawY,
                         LIBRARY_SPIDER_DRAW_W, LIBRARY_SPIDER_DRAW_H,
                         l3SpiderFrames[l3SpiderAnimFrame[i]], false, l3SpiderX[i]);
    }

    if (ceilingHangState == CEILING_GROUND)
    {
        drawPlayer();
    }
    else
    {
        double progress = clampD(ceilingHangTimer / CEILING_CLIMB_TIME, 0.0, 1.0);
        if (ceilingHangState == CEILING_CLIMBING_DOWN) progress = 1.0 - progress;
        double hangY = GROUND_Y + progress * (CEILING_HANG_Y - GROUND_Y);
        if (ceilingAttackActive) hangY -= 200.0;
        // Match the hanging character's body scale to the normal Level 3
        // standing sprite instead of using the source image's large width.
        int hangW = ceilingAttackActive ? 230 : (L3_SPRITE_DRAW_W + 25) * 2;
        int hangH = ceilingAttackActive ? 360 : 128;
        int hangX = (int)assassin.x - hangW / 2;
        unsigned int hangTexture = ceilingAttackActive ? ceilingAttackTexture : ceilingHangTexture;
        iShowImage(hangX, (int)hangY, hangW, hangH, hangTexture);
    }

    double rubbleX[] = { 400.0, 750.0, 750.0, 690.0, 620.0, 560.0, 500.0, 430.0 };
    double rubbleY[] = { 50.0,  50.0, 190.0, 150.0, 195.0, 140.0, 170.0, 120.0 };
    iSetColor(35, 20, 15);
    iFilledPolygon(rubbleX, rubbleY, 8);

    double brickX[] = { 410.0, 740.0, 740.0, 685.0, 620.0, 565.0, 505.0, 440.0 };
    double brickY[] = { 50.0,  50.0, 180.0, 145.0, 185.0, 135.0, 160.0, 115.0 };
    iSetColor(115, 65, 45);
    iFilledPolygon(brickX, brickY, 8);

    iSetColor(140, 80, 50);
    iFilledRectangle(530, 100, 45, 20); iFilledRectangle(590, 75, 50, 22);
    iFilledRectangle(660, 110, 40, 18); iFilledRectangle(690, 60, 45, 25);
    iFilledRectangle(490, 70, 35, 20);

    iSetColor(40, 20, 15);
    iRectangle(530, 100, 45, 20); iRectangle(590, 75, 50, 22);
    iRectangle(660, 110, 40, 18); iRectangle(690, 60, 45, 25);
    iRectangle(490, 70, 35, 20);

    if (roomGuard.active)
    {
        double drawX = roomGuard.x - GUARD_DRAW_W / 2.0;

        if (roomGuard.isDead)
        {
            double deadY = roomGuard.y + GUARD_DEAD_Y_OFFSET;
            drawImageFlipped(drawX, deadY, GUARD_DRAW_W, GUARD_DRAW_H, guardDeadTex, false, roomGuard.x);
        }
        else
        {
            double drawY = roomGuard.y + GUARD_Y_OFFSET;
            bool flipGuard = roomGuard.retreating;
            drawImageFlipped(drawX, drawY, GUARD_DRAW_W, GUARD_DRAW_H, guardFrames[roomGuard.animFrame], flipGuard, roomGuard.x);
        }
    }

    if (hasSecurityKey)
    {
        iSetColor(0, 255, 255);
        iText(15, WIN_H - 130, ">>> SECURITY KEY OBTAINED <<<", GLUT_BITMAP_HELVETICA_18);
    }

    if (ambushTriggered)
    {
        iSetColor(255, 60, 60);
        iText(WIN_W / 2 - 120, WIN_H - 150, "! AMBUSH TRIGGERED !", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (roomGuard.retreating)
    {
        iSetColor(255, 60, 60);
        iText((int)roomGuard.x - 40, (int)roomGuard.y + 260, "! SPOTTED !", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (isHiding)
    {
        iSetColor(100, 255, 100);
        iText((int)assassin.x - 20, (int)assassin.y + 220, "* HIDDEN *", GLUT_BITMAP_HELVETICA_18);
    }
    else if (roomGuard.active && !roomGuard.isDead)
    {
        if (assassin.x > 480.0 && assassin.x < 720.0)
        {
            iSetColor(255, 200, 50);
            iText((int)assassin.x - 70, (int)assassin.y + 220, "Hold 'S' to Hide behind rubble!", GLUT_BITMAP_HELVETICA_18);
        }
    }
    else if ((!roomGuard.active || roomGuard.isDead) && !ambushTriggered && roomGuard.delayTimer <= 0.0)
    {
        if (hasSecurityKey)
        {
            iSetColor(100, 255, 100);
            iText(WIN_W - 250, 30, "Key Secured! Walk Right --->", GLUT_BITMAP_HELVETICA_18);
        }
        else if (isKeyDropped)
        {
            iSetColor(255, 200, 50);
            iText(WIN_W - 220, 30, "Grab the Key!", GLUT_BITMAP_HELVETICA_18);
        }
        else
        {
            iSetColor(100, 255, 100);
            iText(WIN_W - 220, 30, "Coast is Clear...", GLUT_BITMAP_HELVETICA_18);
        }
    }

    drawGameplayHud();
}

void drawLevel3VaultIntroScreen()
{
    iShowImage(0, 0, WIN_W, WIN_H, level3VaultTex[vaultIntroShot]);

    double alpha = 0.0;
    if (vaultIntroTimer < 0.5) alpha = 1.0 - (vaultIntroTimer / 0.5);
    else if (vaultIntroTimer > VAULT_SHOT_TIME - 0.5) alpha = (vaultIntroTimer - (VAULT_SHOT_TIME - 0.5)) / 0.5;

    if (alpha > 0.0)
    {
        beginTransparency();
        setColorAlpha(0, 0, 0, alpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }

    iSetColor(190, 190, 190);
    iText(WIN_W - 150, 16, "Click to skip", GLUT_BITMAP_HELVETICA_12);
}

// ---------------------------------------------------------
// FINAL VAULT ROOM (3_11 AND 3_12)
// ---------------------------------------------------------
void drawLevel3VaultScreen()
{
    if (!isVaultOpen)
    {
        iShowImage(0, 0, WIN_W, WIN_H, level3VaultTex[2]); // 3_11.jpeg (Closed Vault)

        if (hasSecurityKey && assassin.x > 300.0 && assassin.x < 500.0)
        {
            iSetColor(255, 200, 50);
            iText(WIN_W / 2 - 90, WIN_H / 2 - 20, "Press 'X' to Open", GLUT_BITMAP_HELVETICA_18);
        }
    }
    else
    {
        iShowImage(0, 0, WIN_W, WIN_H, level3VaultTex[3]); // 3_12.jpeg (Open Vault)

        beginTransparency();
        setColorAlpha(0, 0, 0, 0.62);
        iFilledRectangle(0, 150, WIN_W, 260);
        endTransparency();

        iSetColor(255, 215, 80);
        iText(WIN_W / 2 - 190, 330, "LEVEL 3 COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 245, 190);
        iText(WIN_W / 2 - 245, 285, "You passed level 3! move to level 4!", GLUT_BITMAP_HELVETICA_18);
        iSetColor(230, 210, 175);
        iText(WIN_W / 2 - 180, 245, "Mission accomplished.", GLUT_BITMAP_HELVETICA_18);
    }

    drawPlayer();

    drawGameplayHud();
}

// ---------------------------------------------------------
// LEVEL 4 - IMPERIAL PALACE ENEMIES (one active at a time)
// ---------------------------------------------------------
void drawLevel4Enemies()
{
    switch (currentLevel4Room())
    {
    case L4_GRAND_ATRIUM_1: case L4_GRAND_ATRIUM_2: case L4_GRAND_ATRIUM_3:
    case L4_COLONADE_1:     case L4_COLONADE_2:     case L4_COLONADE_3:
    {
        if (!katanaFoe.active) break;
        double centerX = katanaFoe.x;
        double drawX = centerX - NINJA_DRAW_W / 2.0;
        double drawY = katanaFoe.y + NINJA_Y_OFFSET;
        bool flip = (katanaFoe.x < assassin.x);
        if (katanaFoe.alive)
        {
            drawImageFlipped(drawX, drawY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaRunFrames[katanaFoe.animFrame], flip, centerX);
            drawEnemyHpBar(centerX, drawY + NINJA_DRAW_H, katanaFoe.hp, KATANA_FOE_MAX_HP);
        }
        else
        {
            drawImageFlipped(drawX, drawY, NINJA_DRAW_W, NINJA_DRAW_H, ninjaDeadTex, flip, centerX);
        }
        break;
    }
    case L4_TREASURY_1: case L4_TREASURY_2: case L4_TREASURY_3:
    {
        if (!hallwayFoe.active) break;
        double centerX = hallwayFoe.x;
        double drawX = centerX - HALLWAY_FOE_DRAW_W / 2.0;
        double drawY = hallwayFoe.y + HALLWAY_FOE_Y_OFFSET;
        bool flip = (hallwayFoe.x < assassin.x);
        if (hallwayFoe.alive)
        {
            drawImageFlipped(drawX, drawY, HALLWAY_FOE_DRAW_W, HALLWAY_FOE_DRAW_H, hallwayWalkFrames[hallwayFoe.animFrame], flip, centerX);
            drawEnemyHpBar(centerX, drawY + HALLWAY_FOE_DRAW_H, hallwayFoe.hp, HALLWAY_FOE_MAX_HP);
        }
        else
        {
            drawImageFlipped(drawX, drawY, HALLWAY_FOE_DRAW_W, HALLWAY_FOE_DRAW_H, hallwayDeadTex, flip, centerX);
        }
        break;
    }
    case L4_SECRET_ARCHIVES_1: case L4_SECRET_ARCHIVES_2: case L4_SECRET_ARCHIVES_3:
    {
        if (!spider.active || !spider.alive) break;
        double centerX = spider.x;
        double drawX = centerX - LIBRARY_SPIDER_DRAW_W / 2.0;
        double drawY = spider.y + LIBRARY_SPIDER_Y_OFFSET;
        bool flip = (spider.x < assassin.x);
        drawImageFlipped(drawX, drawY, LIBRARY_SPIDER_DRAW_W, LIBRARY_SPIDER_DRAW_H, spiderWalkFrames[spider.animFrame], flip, centerX);
        drawEnemyHpBar(centerX, drawY + LIBRARY_SPIDER_DRAW_H, spider.hp, SPIDER_MAX_HP);
        break;
    }
    case L4_ARMORY_1: case L4_ARMORY_2:
    {
        if (!armoryGuard.active) break;
        double centerX = armoryGuard.x;
        double drawX = centerX - GUARD_DRAW_W / 2.0;
        double drawY = armoryGuard.y + GUARD_Y_OFFSET;
        if (armoryGuard.alive)
        {
            // Facing convention: guardFrames faces LEFT by default,
            // so flip=true (armoryGuard.facingRight) mirrors it to
            // face right - see the note at the top of Enemy.h
            // section 9.
            drawImageFlipped(drawX, drawY, GUARD_DRAW_W, GUARD_DRAW_H, guardFrames[armoryGuard.animFrame], armoryGuard.facingRight, centerX);
            drawEnemyHpBar(centerX, drawY + GUARD_DRAW_H, armoryGuard.hp, ARMORY_GUARD_MAX_HP);
        }
        else
        {
            double deadY = armoryGuard.y + GUARD_DEAD_Y_OFFSET;
            drawImageFlipped(drawX, deadY, GUARD_DRAW_W, GUARD_DRAW_H, guardDeadTex, armoryGuard.facingRight, centerX);
        }
        break;
    }
    case L4_IMPERIAL_THRONE_1:
    {
        if (!boss.active) break;
        double centerX = boss.x;
        double drawX = centerX - BOSS_DRAW_W / 2.0;
        double drawY = boss.y + BOSS_Y_OFFSET;
        bool flip = (boss.x < assassin.x);
        if (boss.bState == BOSS_DEAD)
        {
            drawImageFlipped(drawX, drawY, BOSS_DRAW_W, BOSS_DRAW_H, bossDeadTex, flip, centerX);
        }
        else
        {
            int f = clampI(boss.animFrame, 0, BOSS_WALK_FRAME_COUNT - 1);
            drawImageFlipped(drawX, drawY, BOSS_DRAW_W, BOSS_DRAW_H, bossWalkFrames[f], flip, centerX);
            drawEnemyHpBar(centerX, drawY + BOSS_DRAW_H, boss.hp, BOSS_MAX_HP);
        }
        break;
    }
    default:
        break;   // imperial throne 2 (victory screen) / 3 - no enemy
    }
}

// ---------------------------------------------------------
// ARMORY - crate obstacle the player hides behind (see
// ARMORY_HIDE_ZONE_MIN_X/MAX_X in Level4.h). Drawn with simple
// shapes, same approach as the rubble pile in
// drawLevel3Interior2Screen() above.
// ---------------------------------------------------------
void drawArmoryHidingCrates()
{
    int cx = (int)((ARMORY_HIDE_ZONE_MIN_X + ARMORY_HIDE_ZONE_MAX_X) / 2.0);
    int baseY = (int)L4_GROUND_Y;

    iSetColor(90, 60, 35);
    iFilledRectangle(cx - 90, baseY, 60, 55);
    iFilledRectangle(cx - 25, baseY, 60, 72);
    iFilledRectangle(cx + 35, baseY, 55, 45);

    iSetColor(50, 30, 15);
    iRectangle(cx - 90, baseY, 60, 55);
    iRectangle(cx - 25, baseY, 60, 72);
    iRectangle(cx + 35, baseY, 55, 45);

    iSetColor(70, 45, 25);
    iLine(cx - 90, baseY + 27, cx - 30, baseY + 27);
    iLine(cx - 25, baseY + 36, cx + 35, baseY + 36);
    iLine(cx + 35, baseY + 22, cx + 90, baseY + 22);
}

// ---------------------------------------------------------
// LEVEL 4 - IMPERIAL PALACE ROOMS
// ---------------------------------------------------------
// Screened like Level 3's intro cinematic: black letterbox bars
// top/bottom, and a quick fade-up from black whenever a new
// background first appears (level4StepTimer resets to 0 in
// Level4.h's advanceLevel4Room()).
void drawWoodenAtriumCeiling()
{
    const int ceilingH = 90;
    const int ceilingBottom = WIN_H - ceilingH;

    // Opaque base makes the ceiling clearly visible over either atrium image.
    iSetColor(70, 36, 18);
    iFilledRectangle(0, ceilingBottom, WIN_W, ceilingH);

    // Individual wooden boards, made only with filled rectangles.
    for (int row = 0; row < 2; row++)
    {
        int y = ceilingBottom + row * 42 + 5;
        int offset = (row % 2) ? -70 : 0;
        for (int x = offset; x < WIN_W; x += 180)
        {
            iSetColor((row % 2) ? 126 : 145, (row % 2) ? 67 : 78, 32);
            iFilledRectangle(x + 3, y, 174, 36);

            iSetColor(82, 39, 18);
            iFilledRectangle(x + 3, y, 174, 4);
            iFilledRectangle(x + 3, y + 32, 174, 4);

            // Small grain marks on each board.
            iSetColor(178, 98, 42);
            iFilledRectangle(x + 28, y + 17, 45, 3);
            iFilledRectangle(x + 105, y + 10, 28, 3);
        }
    }

    // Large cross beams make the ceiling read as a wooden structure.
    iSetColor(53, 25, 12);
    iFilledRectangle(0, ceilingBottom + 1, WIN_W, 14);
    iFilledRectangle(0, WIN_H - 16, WIN_W, 16);
    iFilledRectangle(178, ceilingBottom, 18, ceilingH);
    iFilledRectangle(528, ceilingBottom, 18, ceilingH);

    iSetColor(181, 101, 42);
    iFilledRectangle(0, ceilingBottom + 15, WIN_W, 4);
    iFilledRectangle(0, WIN_H - 21, WIN_W, 4);
}

void drawLevel4Screen()
{
    if (l4StairIntroActive)
    {
        iShowImage(0, 0, WIN_W, WIN_H, level4StairIntroTexture);
        // Always go through the normal drawPlayer()/pickPlayerSprite()
        // path here, same as every other Level 4 room. The previous
        // version of this block hand-drew a sprite directly using
        // walkRightFrames[] (the unarmed walk set, sized for the L3
        // idle pose) whenever assassin.dx != 0 - despite the comment
        // above claiming it used the katana running frames. Since
        // assassin.walkFrame is now capped to KATANA_RUN_FRAME_COUNT
        // (see updatePlayerWalkAnimation() in Player.h), that stray
        // "% WALK_FRAME_COUNT" only ever showed the first few frames
        // of the WRONG, wrong-sized animation. drawPlayer() already
        // selects katanaRunFrames[] at the correct KATANA_RUN_DRAW_W/H
        // size via pickPlayerSprite() whenever the player is moving
        // with the katana equipped (isKatanaAttacking, isJumping, etc.
        // are all handled there too), so there's no need to duplicate
        // any of that logic here.
        drawPlayer();
        drawGameplayHud();
        iSetColor(230, 210, 175);
        iText(WIN_W / 2 - 170, WIN_H - 38, "A/D to climb the stairs", GLUT_BITMAP_HELVETICA_18);

        // --- TEMPORARY DEBUG LINES - remove once the stair-climb
        // animation bug is found. These are ACCUMULATED across every
        // tick since the stairs began (see dbgTotalTicks etc. in
        // Level4.h), so unlike a single instantaneous reading, a
        // screenshot taken at ANY moment shows the true history -
        // it can't land on a lucky/unlucky single tick.
        {
            char dbg1[220], dbg2[220];
            sprintf(dbg1, "DEBUG now: dx=%.2f moving=%d katana=%d walkFrame=%d",
                assassin.dx, playerIsMoving() ? 1 : 0, isKatanaEquipped ? 1 : 0, assassin.walkFrame);
            sprintf(dbg2, "DEBUG history: totalTicks=%d movingTicks=%d maxDx=%.2f framesSeenMask=%d",
                dbgTotalTicks, dbgMovingTicks, dbgMaxAbsDx, dbgFramesSeenMask);
            iSetColor(255, 60, 60);
            iText(20, WIN_H - 60, dbg1, GLUT_BITMAP_HELVETICA_18);
            iText(20, WIN_H - 85, dbg2, GLUT_BITMAP_HELVETICA_18);
        }

        return;
    }

    iShowImage(0, 0, WIN_W, WIN_H, level4RoomTex[currentLevel4Room()]);

    drawLevel4Enemies();
    drawPlayer();

    // The Armory's crates are drawn AFTER the player (same
    // ordering Level 3 uses for its rubble pile), so they visually
    // cover the player while hidden behind them.
    if (isArmoryRoom(currentLevel4Room()))
        drawArmoryHidingCrates();

    // --- Level 3-style screening: letterbox bars ---
    const int barH = 46;
    iSetColor(0, 0, 0);
    iFilledRectangle(0, WIN_H - barH, WIN_W, barH);
    iFilledRectangle(0, 0, WIN_W, barH);

    // --- Level 3-style screening: fade up from black on a new shot ---
    if (level4StepTimer < L4_FADE_IN_TIME)
    {
        double curtainAlpha = 1.0 - (level4StepTimer / L4_FADE_IN_TIME);
        beginTransparency();
        setColorAlpha(0, 0, 0, curtainAlpha);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();
    }

    drawGameplayHud();

    if (isHiding)
    {
        iSetColor(100, 255, 100);
        iText((int)assassin.x - 20, (int)assassin.y + 220, "* HIDDEN *", GLUT_BITMAP_HELVETICA_18);
    }
    else if (isArmoryRoom(currentLevel4Room()) && armoryGuard.active && armoryGuard.alive)
    {
        bool inZone = (assassin.x > ARMORY_HIDE_ZONE_MIN_X && assassin.x < ARMORY_HIDE_ZONE_MAX_X);
        if (inZone)
        {
            iSetColor(255, 200, 50);
            iText((int)assassin.x - 100, (int)assassin.y + 230, "Hold 'S' to Hide behind crates!", GLUT_BITMAP_HELVETICA_18);
        }
    }

    // --- IMPERIAL THRONE 2: victory screen - the Emperor has fallen ---
    if (currentLevel4Room() == L4_IMPERIAL_THRONE_2)
    {
        beginTransparency();
        setColorAlpha(0, 0, 0, 0.45);
        iFilledRectangle(0, 0, WIN_W, WIN_H);
        endTransparency();

        iSetColor(255, 215, 90);
        iText(WIN_W / 2 - 190, WIN_H / 2 + 100, "YOU HAVE WON!", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 245, 190);
        iText(WIN_W / 2 - 250, WIN_H / 2 + 58, "THE EMPEROR HAS FALLEN", GLUT_BITMAP_HELVETICA_18);
        iSetColor(230, 210, 175);
        iText(WIN_W / 2 - 285, WIN_H / 2 + 25, "Silent Pursuit is complete.", GLUT_BITMAP_HELVETICA_18);

        drawButton(btnGameCompleted);
        return;   // skip the normal walk/weapon hints below - the fight is over
    }

    bool bossAlive = (foeKindForRoom(currentLevel4Room()) == L4_FOE_BOSS && boss.active && boss.bState != BOSS_DEAD);

    iSetColor(210, 205, 195);
    if (bossAlive)
        iText(WIN_W / 2 - 165, 30, "Defeat the Emperor to proceed!", GLUT_BITMAP_HELVETICA_18);
    else if (level4CurrentStep < LEVEL4_SEQUENCE_LENGTH - 1)
        iText(WIN_W - 260, 30, "Walk Right or Press SPACE --->", GLUT_BITMAP_HELVETICA_18);

    iSetColor(210, 205, 195);
    iText(15, 30, isKnifeEquipped ? "A/D to run | LEFT CLICK to swing Knife | Hold S to hide | SPACE to continue"
        : "A/D to run | LEFT CLICK to swing Katana | Hold S to hide | SPACE to continue", GLUT_BITMAP_HELVETICA_12);
}

void drawGameOverScreen()
{
    iShowImage(0, 0, WIN_W, WIN_H, level1BgTexture);
    beginTransparency(); setColorAlpha(0, 0, 0, 0.72); iFilledRectangle(0, 0, WIN_W, WIN_H); endTransparency();
    iSetColor(220, 40, 30); iText(WIN_W / 2 - 95, 430, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(220, 210, 200);
    if (playerHP <= 0 && killedByMine) iText(WIN_W / 2 - 165, 390, "You were killed by a hidden landmine.", GLUT_BITMAP_HELVETICA_18);
    else if (playerHP <= 0) iText(WIN_W / 2 - 105, 390, "You were killed in combat.", GLUT_BITMAP_HELVETICA_18);
    else iText(WIN_W / 2 - 145, 390, "You fell into the ravine below the bridge.", GLUT_BITMAP_HELVETICA_18);
    drawButton(btnRetry); drawButton(btnGoToMenu);
}
#endif