#include "iGraphics.h"
#include "Config.h"
#include "Audio.h"
#include "Draw.h"
#include "Assets.h"
#include "World.h"
#include "Effects.h"
#include "UI.h"
#include "Player.h"
#include "Enemy.h"
#include "Landmine.h"
#include "Level1.h"
#include "Level3.h"
#include "Level4.h"
#include "Level2Runtime.h"
#include "Screens.h"



void iDraw()
{
    iClear();
    if (state == LOADING)            drawLoadingScreen();
    else if (state == MENU)          drawMenuScreen();
    else if (state == LEVEL_SELECT)  drawLevelSelectScreen();
    else if (state == OPTIONS)       drawOptionsScreen();
    else if (state == CREDITS)       drawCreditsScreen();
    else if (state == GAMEPLAY)      { if (currentLevel == 2) drawLevel2Screen(); else drawGameplayScreen(); }
    else if (state == GAME_OVER)     { if (currentLevel == 2) drawLevel2GameOverScreen(); else drawGameOverScreen(); }
    else if (state == LEVEL3_INTRO)  drawLevel3IntroScreen();
    else if (state == LEVEL3)        drawLevel3Screen();
    else if (state == LEVEL3_INTERIOR) drawLevel3InteriorScreen();
    else if (state == LEVEL3_TRANSITION) drawLevel3TransitionScreen();
    else if (state == LEVEL3_INTERIOR_2) drawLevel3Interior2Screen();
    else if (state == LEVEL3_TIGER) drawLevel3TigerScreen();
    else if (state == LEVEL3_VAULT_INTRO) drawLevel3VaultIntroScreen();
    else if (state == LEVEL3_VAULT)  drawLevel3VaultScreen();
    else if (state == LEVEL4)        drawLevel4Screen();   // Level 4 - Imperial Palace rooms
}

void iMouseMove(int mx, int my) { int lx, ly; toLogicalCoords(mx, my, lx, ly); if (state == GAMEPLAY) updateCrosshair(lx, ly); }
void iPassiveMouseMove(int mx, int my)
{
    int lx, ly; toLogicalCoords(mx, my, lx, ly);
    if (state == MENU) { btnNewGame.hovered = isInside(btnNewGame, lx, ly); btnOptions.hovered = isInside(btnOptions, lx, ly); btnCredits.hovered = isInside(btnCredits, lx, ly); btnQuit.hovered = isInside(btnQuit, lx, ly); }
    else if (state == LEVEL_SELECT) { btnLevel1.hovered = isInside(btnLevel1, lx, ly); btnLevel2.hovered = isInside(btnLevel2, lx, ly); btnLevel3.hovered = isInside(btnLevel3, lx, ly); btnLevel4.hovered = isInside(btnLevel4, lx, ly); btnLvlBack.hovered = isInside(btnLvlBack, lx, ly); }
    else if (state == OPTIONS) { btnAudio.hovered = isInside(btnAudio, lx, ly); btnControls.hovered = isInside(btnControls, lx, ly); btnOptBack.hovered = isInside(btnOptBack, lx, ly); }
    else if (state == CREDITS) { btnCredBack.hovered = isInside(btnCredBack, lx, ly); }
    else if (state == GAMEPLAY) { updateCrosshair(lx, ly); }
    else if (state == GAME_OVER) { btnRetry.hovered = isInside(btnRetry, lx, ly); btnGoToMenu.hovered = isInside(btnGoToMenu, lx, ly); }
    else if (state == LEVEL4) { btnGameCompleted.hovered = isInside(btnGameCompleted, lx, ly); }  // only shown/clickable on Imperial Throne 2
}

void iMouse(int button, int mstate, int mx, int my)
{
    int lx, ly; toLogicalCoords(mx, my, lx, ly);
    if (button == GLUT_LEFT_BUTTON && mstate == GLUT_DOWN)
    {
        if (state == MENU) { if (isInside(btnNewGame, lx, ly)) state = LEVEL_SELECT; else if (isInside(btnOptions, lx, ly)) state = OPTIONS; else if (isInside(btnCredits, lx, ly)) state = CREDITS; else if (isInside(btnQuit, lx, ly)) exit(0); }
        else if (state == LEVEL_SELECT)
        {
            if (isInside(btnLevel1, lx, ly) && !btnLevel1.locked) { stopGameAudioChannel("loading_audio"); loadingAudioPlaying = false; currentLevel = 1; resetLevel1(); state = GAMEPLAY; }
            else if (isInside(btnLevel2, lx, ly) && !btnLevel2.locked) { stopGameAudioChannel("loading_audio"); loadingAudioPlaying = false; resetLevel2(); state = GAMEPLAY; }
            else if (isInside(btnLevel3, lx, ly) && !btnLevel3.locked) { stopGameAudioChannel("loading_audio"); loadingAudioPlaying = false; currentLevel = 3; resetLevel3Intro(); state = LEVEL3_INTRO; }
            else if (isInside(btnLevel4, lx, ly) && !btnLevel4.locked) { stopGameAudioChannel("loading_audio"); loadingAudioPlaying = false; currentLevel = 4; resetLevel4(); state = LEVEL4; }
            else if (isInside(btnLvlBack, lx, ly)) state = MENU;
        }
        else if (state == LEVEL3_INTRO) skipLevel3Intro();
        else if (state == LEVEL3_VAULT_INTRO) skipLevel3VaultIntro();
        else if (state == OPTIONS) { if (isInside(btnOptBack, lx, ly)) state = MENU; }
        else if (state == CREDITS) { if (isInside(btnCredBack, lx, ly)) state = MENU; }
        else if (state == GAME_OVER) { if (isInside(btnRetry, lx, ly)) { if (currentLevel == 2) resetLevel2(); else { currentLevel = 1; resetLevel1(); } state = GAMEPLAY; } else if (isInside(btnGoToMenu, lx, ly)) state = MENU; }
        else if (state == LEVEL4)
        {
            // Only clickable/relevant on the Imperial Throne 2 victory
            // screen (see drawLevel4Screen() in Screens.h) - harmless
            // no-op everywhere else in Level 4.
            if (currentLevel4Room() == L4_IMPERIAL_THRONE_2 && isInside(btnGameCompleted, lx, ly))
                state = MENU;
        }
    }

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GAMEPLAY) {
            if (mstate == GLUT_DOWN) {
                isFiring = true;
                keepMouseActive();
                if (gunEquipped)
                    playGameAudioChannel("GUN_FIRING_SOUND.mp3", "gun_audio");
            }
            else if (mstate == GLUT_UP) {
                isFiring = false;
                stopGameAudioChannel("gun_audio");
            }
        }
        else if (state == LEVEL3) {
            if (mstate == GLUT_DOWN && level3Phase == L3_READY && !isRpgFired)
            {
                isRpgFired = true;
                playGameAudio("RPG_SOUND.mp3");
            }
        }
        else if (state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4)
        {
            if (mstate == GLUT_DOWN) {
                isKatanaHeld = true; // Mouse is being held
                if (state == LEVEL3_INTERIOR_2 && isCeilingHanging()) {
                    startCeilingAttack();
                }
                else if (!isKatanaAttacking && !isHiding) {
                    isKatanaAttacking = true;
                    katanaAttackFrame = 0;
                    katanaAttackTimer = 0.0;
                    hasPlayerDealtDamageThisSwing = false;
                    playGameAudio("KATANA_SOUND.mp3");
                }
            }
            else if (mstate == GLUT_UP) {
                isKatanaHeld = false; // Mouse was released
            }
        }
    }
}

void iKeyboard(unsigned char key)
{
    if (key == 'a' || key == 'A') movementAKeyDown = true;
    if (key == 'd' || key == 'D') movementDKeyDown = true;

    // Make A/D immediately responsive when entering the tiger atrium. The
    // held-key state continues the movement in updateLevel3Tiger().
    if (state == LEVEL3_TIGER)
    {
        if ((key == 'a' || key == 'A') && !movementDKeyDown)
            assassin.x -= L3_WALK_SPEED;
        else if ((key == 'd' || key == 'D') && !movementAKeyDown)
            assassin.x += L3_WALK_SPEED;
        assassin.x = clampD(assassin.x, 80.0, WIN_W - 80.0);
    }

    if (key == 27) { if (state == OPTIONS || state == CREDITS || state == LEVEL_SELECT || state == LEVEL3 || state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4 || (state == GAMEPLAY && currentLevel == 2)) state = MENU; }
    if (state == GAME_OVER && (key == 'r' || key == 'R')) { if (currentLevel == 2) resetLevel2(); else { currentLevel = 1; resetLevel1(); } state = GAMEPLAY; }
    if (state == GAMEPLAY || state == LEVEL3_INTERIOR || state == LEVEL3_INTERIOR_2 || state == LEVEL3_VAULT || state == LEVEL4) { if (key == 'h' || key == 'H') showHitBoxes = !showHitBoxes; }
    if (state == LEVEL3_INTRO) skipLevel3Intro();
    if (state == LEVEL3_VAULT_INTRO) skipLevel3VaultIntro();
}

void keyRelease(unsigned char key, int x, int y)
{
    if (key == 'a' || key == 'A') movementAKeyDown = false;
    if (key == 'd' || key == 'D') movementDKeyDown = false;
}
void iSpecialKeyboard(unsigned char key) {}
void specialKeyRelease(int key, int x, int y) {}
    
void fixedUpdate()
{
    animTime += TICK;

    if (state == LOADING && !loadingAudioPlaying)
    {
        playGameAudioChannel("LOADING_AUDIO.mp3", "loading_audio", true);
        loadingAudioPlaying = true;
    }

    if (state == LOADING && !loadingDone) { loadingProgress++; if (loadingProgress >= 100) { loadingProgress = 100; loadingDone = true; state = MENU; } }
    else if (state == GAMEPLAY)
    {
        if (currentLevel == 2)
        {
            updateLevel2();
            if (level2ExitActivated && isKeyPressed(' '))
            {
                currentLevel = 3;
                btnLevel3.locked = false;
                sprintf(btnLevel3.label, "Level 3");
                resetLevel3Intro();
                state = LEVEL3_INTRO;
            }
        }
        else updateLevel1();
    }
    else if (state == LEVEL3_INTRO) { updateLevel3Intro(); if (level3IntroDone) { resetLevel3(); state = LEVEL3; } }
    else if (state == LEVEL3) updateLevel3();
    else if (state == LEVEL3_INTERIOR) updateLevel3Interior();
    else if (state == LEVEL3_TRANSITION) updateLevel3Transition();
    else if (state == LEVEL3_INTERIOR_2) updateLevel3Interior2();
    else if (state == LEVEL3_TIGER) updateLevel3Tiger();
    else if (state == LEVEL3_VAULT_INTRO) updateLevel3VaultIntro();
    else if (state == LEVEL3_VAULT)  updateLevel3Vault();
    else if (state == LEVEL4)        updateLevel4();   // Level 4 - Imperial Palace rooms

    bool playerRunning = playerIsMoving() &&
        (state == GAMEPLAY || state == LEVEL3 || state == LEVEL3_INTERIOR ||
         state == LEVEL3_INTERIOR_2 || state == LEVEL3_TIGER || state == LEVEL3_VAULT ||
         state == LEVEL4);
    if (playerRunning && !runningAudioPlaying)
    {
        playGameAudioChannel("RUNNING.mp3", "running_audio", true);
        runningAudioPlaying = true;
    }
    else if (!playerRunning && runningAudioPlaying)
    {
        stopGameAudioChannel("running_audio");
        runningAudioPlaying = false;
    }

    // Do not regenerate health from EP during the player's attack animation.
    // Updating EP after gameplay lets the current combat state take effect
    // immediately instead of one frame later.
    bool level2Gameplay = state == GAMEPLAY && currentLevel == 2;
    epConversionPaused = isKatanaAttacking || level2Gameplay;
    updateEP();
}

int main()
{
    iInitialize(WIN_W, WIN_H, "Silent Pursuit");
    glutIgnoreKeyRepeat(0);
    loadAllAssets();
    resetAllEnemies();
    iSetTimer(TIMER_INTERVAL_MS, fixedUpdate);
    glutKeyboardUpFunc(keyRelease);
    glutSpecialUpFunc(specialKeyRelease);
    iStart();
    return 0;
}