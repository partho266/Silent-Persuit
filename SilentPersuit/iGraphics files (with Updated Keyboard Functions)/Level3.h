#ifndef LEVEL3_H
#define LEVEL3_H

#include "Config.h"
#include "Assets.h"   
#include "Player.h"   
#include "Enemy.h"    
#include "stdlib.h"   

enum Level3Phase { L3_START, L3_LIFT_1, L3_LIFT_2, L3_LIFT_3, L3_READY };

const double L3_START_TIME = 0.8;        
const double L3_LIFT_FRAME_TIME = 0.35;  
const double L3_BLAST_FRAME_TIME = 0.2;  

Level3Phase level3Phase = L3_START;
double level3PhaseTimer = 0.0;
double l3TransitionTimer = 0.0; 

bool isRpgFired = false;
double blastTimer = 0.0;
int blastState = 0; 
bool level3GameplayActive = false;
const double L3_WALK_SPEED = 3.0;

// ---------------------------------------------------------
// SECURITY ALARM (triggers once the player steps through the
// RPG-blasted door into the interior)
// ---------------------------------------------------------
double securityShakeTimer = 0.0;
double l3WarningFlashTimer = 0.0;
const double SECURITY_SHAKE_DURATION = 5.0; // seconds the alarm shake/overlay lasts
const double SECURITY_SHAKE_STRENGTH = 3.0; // pixels of jitter (kept subtle)
const double FIRST_NINJA_SPAWN_DELAY = 1.0; // spawn during the first second of the alarm
const double L3_NINJA_SPEED = 4.5;
const double L3_GUARD_SPEED = 1.6; 

// ---------------------------------------------------------
// TIME BOMB: after killing the first two ninjas the player
// must reach the next chamber within BOMB_COUNTDOWN seconds
// or be killed by the explosion.
// ---------------------------------------------------------
bool l3BombActive = false;
bool l3DoorSceneAudioPlaying = false;
double l3BombTimer = 0.0;
// Time available after the second ninja is killed.
const double BOMB_COUNTDOWN = 4.0;
// Bomb explosion visual state
bool l3BombTriggered = false;
double l3BombExplTimer = 0.0;
double l3BombFlashTimer = 0.0;
const double L3_BOMB_FLASH_DURATION = 0.18;
const double L3_BOMB_SHAKE_TIME = 0.9;
const double L3_BOMB_SHAKE_POWER = 20.0;
const double L3_TEAR_FRAME_TIME = 0.85;
const double L3_DEATH_MESSAGE_TIME = 4.0;
bool l3TearSequenceActive = false;
double l3TearSequenceTimer = 0.0;
int l3TearFrame = 0;
bool l3DeathMessageActive = false;
double l3DeathMessageTimer = 0.0;

const int MAX_DUST_PARTICLES = 60;
struct DustParticle { double x, y; double vx, vy; double radius; double alpha; bool active; };
DustParticle dust[MAX_DUST_PARTICLES];

void spawnDust(int i)
{
    dust[i].x = (WIN_W - 250.0) + (rand() % 100); 
    dust[i].y = 100.0 + (rand() % 250);
    dust[i].vx = -0.5 - ((rand() % 100) / 100.0);
    dust[i].vy = -0.2 + ((rand() % 100) / 50.0);
    dust[i].radius = 15.0 + (rand() % 25);
    dust[i].alpha = 0.25 + ((rand() % 100) / 400.0); 
    dust[i].active = true;
}

void resetLevel3()
{
    level3Phase = L3_START;
    level3PhaseTimer = 0.0;
    isRpgFired = false;
    blastState = 0;
    blastTimer = 0.0;
    l3TransitionTimer = 0.0;
    level3GameplayActive = false;
    securityShakeTimer = 0.0;
    l3WarningFlashTimer = 0.0;
    l3BombActive = false;
    l3BombTimer = 0.0;
    l3BombTriggered = false;
    l3BombExplTimer = 0.0;
    l3BombFlashTimer = 0.0;
    l3TearSequenceActive = false;
    l3TearSequenceTimer = 0.0;
    l3TearFrame = 0;
    l3DeathMessageActive = false;
    l3DeathMessageTimer = 0.0;
    for(int i = 0; i < MAX_DUST_PARTICLES; i++) dust[i].active = false;
}

void initLevel3Interior()
{
    state = LEVEL3_INTERIOR; 
    
    isRpgEquipped = false;
    gunEquipped = false;
    isKatanaEquipped = true;
    isKatanaAttacking = false;
    isKatanaHeld = false; 
    isHiding = false;
    
    assassin.x = 120.0;
    assassin.y = GROUND_Y - 20.0; 
    
    resetNinjas();

    // The blast just blew the door open - the security system
    // kicks in the moment the player steps inside.
    securityShakeTimer = SECURITY_SHAKE_DURATION;
    l3WarningFlashTimer = 0.45;
    playGameAudio("WARNING.mp3");

    // Spawn the first ninja one second after entering the shaking chamber.
    nextNinjaSpawnTimer = FIRST_NINJA_SPAWN_DELAY;
    // Reset any bomb state when entering the interior
    l3BombActive = false;
    l3BombTimer = 0.0;
    l3BombTriggered = false;
    l3BombExplTimer = 0.0;
    l3BombFlashTimer = 0.0;
    l3TearSequenceActive = false;
    l3TearSequenceTimer = 0.0;
    l3TearFrame = 0;
    l3DeathMessageActive = false;
    l3DeathMessageTimer = 0.0;
}

void updateLevel3Gameplay()
{
    bool leftHeld  = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

    if (leftHeld && !rightHeld)      assassin.dx = -L3_WALK_SPEED;
    else if (rightHeld && !leftHeld) assassin.dx = L3_WALK_SPEED;
    else                              assassin.dx = 0.0;

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 60.0, WIN_W - 60.0);   

    toggleGunWithGKey();
    toggleRpgWithFKey();
    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();

    if (assassin.x >= WIN_W - 70.0)
    {
        initLevel3Interior();
    }

}

void updateLevel3()
{
    level3PhaseTimer += TICK;
    switch (level3Phase)
    {
        case L3_START: if (level3PhaseTimer >= L3_START_TIME) { level3Phase = L3_LIFT_1; level3PhaseTimer = 0.0; } break;
        case L3_LIFT_1: if (level3PhaseTimer >= L3_LIFT_FRAME_TIME) { level3Phase = L3_LIFT_2; level3PhaseTimer = 0.0; } break;
        case L3_LIFT_2: if (level3PhaseTimer >= L3_LIFT_FRAME_TIME) { level3Phase = L3_LIFT_3; level3PhaseTimer = 0.0; } break;
        case L3_LIFT_3: if (level3PhaseTimer >= L3_LIFT_FRAME_TIME) { level3Phase = L3_READY; level3PhaseTimer = 0.0; isRpgEquipped = true; } break;
        case L3_READY: break;
    }

    if (isRpgFired)
    {
        if (blastState < 2)
        {
            blastTimer += TICK;
            if (blastTimer >= L3_BLAST_FRAME_TIME) { blastState++; blastTimer = 0.0; }
        }
        else 
        {
            blastTimer += TICK;
            if (!level3GameplayActive)
            {
                level3GameplayActive = true;
                cameraX = 0.0; shakeTimer = 0.0; shakeOffsetX = 0.0;
                assassin.x = WIN_W / 2.0 - 10.0; assassin.y = GROUND_Y;
                assassin.dx = 0.0; assassin.dy = 0.0;
                assassin.isJumping = false; assassin.facingRight = true;
                assassin.walkFrame = 0; assassin.walkFrameTimer = 0.0;
                isRpgEquipped = true; gunEquipped = false;
            }

            for (int i = 0; i < MAX_DUST_PARTICLES; i++)
            {
                if (!dust[i].active)
                {
                    if (blastTimer < 3.0 && rand() % 100 < 5) spawnDust(i);
                }
                else
                {
                    dust[i].x += dust[i].vx; dust[i].y += dust[i].vy; dust[i].radius += 0.1; dust[i].alpha -= 0.0015;  
                    if (dust[i].alpha <= 0.0 || dust[i].x + dust[i].radius < 0.0) dust[i].active = false;
                }
            }
            updateLevel3Gameplay();
        }
    }

}

void updateLevel3Interior()
{
    countDown(securityShakeTimer);
    countDown(l3WarningFlashTimer);

    // The cinematic death message must own the update loop. This prevents
    // movement/exit logic from restarting or interrupting the sequence.
    if (l3DeathMessageActive)
    {
        countDown(l3DeathMessageTimer);
        if (l3DeathMessageTimer <= 0.0)
        {
            l3DeathMessageActive = false;
            l3TearSequenceActive = false;
            l3BombTriggered = false;
            state = GAME_OVER;
        }
        return;
    }

    bool leftHeld  = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

    if (isKatanaAttacking) {
        if (assassin.facingRight) assassin.dx = L3_WALK_SPEED * 0.4;
        else assassin.dx = -L3_WALK_SPEED * 0.4;
    } 
    else if (leftHeld && !rightHeld) assassin.dx = -L3_WALK_SPEED;
    else if (rightHeld && !leftHeld) assassin.dx = L3_WALK_SPEED;
    else assassin.dx = 0.0;

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 60.0, WIN_W - 60.0);   
    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();

    if (assassin.x >= WIN_W - 70.0)
    {
        // If chamber cleared and player moves right, transition and cancel bomb
        bool allDead = true;
        for (int i = 0; i < MAX_NINJAS; i++) if (ninjas[i].active && ninjas[i].state != NINJA_DEAD) allDead = false;
        if (allDead && currentNinjaSpawnIndex >= MAX_NINJAS)
        {
            l3BombActive = false;
            state = LEVEL3_TRANSITION;
            l3TransitionTimer = 0.0;
            return;
        }
    }

    if (nextNinjaSpawnTimer > 0.0)
    {
        nextNinjaSpawnTimer -= TICK;
        if (nextNinjaSpawnTimer <= 0.0) spawnNinja(BLUE_NINJA, WIN_W + 100.0);
    }

    if (isKatanaAttacking)
    {
        katanaAttackTimer += TICK;
        if (katanaAttackTimer > 0.06) 
        {
            katanaAttackTimer = 0.0;
            katanaAttackFrame++;
            
            if (katanaAttackFrame == 2 && !hasPlayerDealtDamageThisSwing)
            {
                for (int i = 0; i < MAX_NINJAS; i++)
                {
                    if (ninjas[i].active && ninjas[i].state != NINJA_DEAD)
                    {
                        if (fabs(assassin.x - ninjas[i].x) < 120.0)
                        {
                            ninjas[i].hp -= 50; 
                            hasPlayerDealtDamageThisSwing = true;
                            
                            if (ninjas[i].hp <= 0) 
                            {
                                ninjas[i].state = NINJA_DEAD;
                                playGameAudio("SCREAM_ON_DEATH_ENEMY_OR_PLAYER.mp3");
                                nextNinjaSpawnTimer = 2.0; 
                                playerEP += 20; // NEW: Grant EP on kill
                            }
                            break; 
                        }
                    }

                    }

            }

            if (katanaAttackFrame >= 5) 
            {
                if (isKatanaHeld && !isHiding) {
                    katanaAttackFrame = 0; 
                    hasPlayerDealtDamageThisSwing = false;
                } else {
                    isKatanaAttacking = false;
                    katanaAttackFrame = 0;
                    hasPlayerDealtDamageThisSwing = false;
                }
            }
        }
    }

    for (int i = 0; i < MAX_NINJAS; i++)
    {
        if (!ninjas[i].active || ninjas[i].state == NINJA_DEAD) continue; 

        if (ninjas[i].state == NINJA_RUNNING)
        {
            ninjas[i].x -= L3_NINJA_SPEED; 
            ninjas[i].animTimer += TICK;
            if (ninjas[i].animTimer > 0.08)
            {
                ninjas[i].animTimer = 0.0;
                ninjas[i].animFrame = (ninjas[i].animFrame + 1) % NINJA_RUN_FRAME_COUNT;
            }

            if (fabs(ninjas[i].x - assassin.x) < 90.0)
            {
                ninjas[i].state = NINJA_ATTACKING;
                ninjas[i].animFrame = 0;
                ninjas[i].animTimer = 0.0;
                ninjas[i].hasDealtDamageThisSwing = false;
            }
        }
        else if (ninjas[i].state == NINJA_ATTACKING)
        {
            ninjas[i].animTimer += TICK;
            if (ninjas[i].animTimer > 0.1) 
            {
                ninjas[i].animTimer = 0.0;
                ninjas[i].animFrame++;

                if (ninjas[i].animFrame == 2 && !ninjas[i].hasDealtDamageThisSwing)
                {
                    playerHP -= 40;
                    ninjas[i].hasDealtDamageThisSwing = true;
                    if (playerHP <= 0) { playerHP = 0; state = GAME_OVER; }
                }

                if (ninjas[i].animFrame >= NINJA_ATTACK_FRAME_COUNT)
                {
                    if (fabs(ninjas[i].x - assassin.x) < 90.0)
                    {
                        ninjas[i].animFrame = 0;
                        ninjas[i].hasDealtDamageThisSwing = false;
                    }
                    else
                    {
                        ninjas[i].state = NINJA_RUNNING;
                        ninjas[i].animFrame = 0;
                    }
                }
            }
        }
    }

    if (!l3BombActive && !l3BombTriggered)
    {
        int deadCount = 0;
        for (int i = 0; i < MAX_NINJAS; i++)
            if (ninjas[i].state == NINJA_DEAD) deadCount++;

        if (deadCount >= 2)
        {
            l3BombActive = true;
            l3BombTimer = BOMB_COUNTDOWN;
            playGameAudio("TIME_BOMB_COUNTDOWN.mp3", true);
            printf("[Level3] Bomb countdown started: %f seconds\n", l3BombTimer);
        }
    }

    if (l3BombActive)
    {
        countDown(l3BombTimer);
        if (l3BombTimer <= 0.0)
        {
            l3BombActive = false;
            stopGameAudio();
            l3BombTriggered = true;
            l3BombExplTimer = EXPLOSION_DURATION;
            l3BombFlashTimer = L3_BOMB_FLASH_DURATION;
            l3TearSequenceActive = true;
            l3TearSequenceTimer = 0.0;
            l3TearFrame = 0;
            playerHP = 0;
            startShake(L3_BOMB_SHAKE_TIME, L3_BOMB_SHAKE_POWER);
            printf("[Level3] Bomb exploded: starting visual sequence\n");
            playGameAudio("TIME_BOMB_EXPLOSION.mp3");
        }
    }

    if (l3BombTriggered)
    {
        countDown(l3BombExplTimer);
        countDown(l3BombFlashTimer);
        updateShake();
        if (l3BombExplTimer <= 0.0)
        {
            l3BombTriggered = false;
        }
    }

    if (l3TearSequenceActive)
    {
        l3TearSequenceTimer += TICK;
        l3TearFrame = (int)(l3TearSequenceTimer / L3_TEAR_FRAME_TIME);
        if (l3TearFrame >= 3)
        {
            l3TearFrame = 2;
            l3TearSequenceActive = false;
            l3DeathMessageActive = true;
            l3DeathMessageTimer = L3_DEATH_MESSAGE_TIME;
        }
        return;
    }

}

// ---------------------------------------------------------
// VAULT DOOR CINEMATIC TIMERS & STATE
// ---------------------------------------------------------
double vaultIntroTimer = 0.0;
int vaultIntroShot = 0;
const double VAULT_SHOT_TIME = 2.5; 
bool isVaultOpen = false; // Tracks if you have pushed the door open!
bool l3CompletionAudioPlayed = false;
bool l3DoorChoiceActive = false;
bool l3WrongDoorMessage = false;
int l3CorrectDoor = 0;
const double L3_DOOR_LEFT_X = 330.0;
const double L3_DOOR_RIGHT_X = 670.0;
double l3TigerX = 40.0;
double l3TigerAnimTimer = 0.0;
int l3TigerAnimFrame = 0;
double l3TigerAttackTimer = 0.0;
double l3TigerFlashTimer = 0.0;
double l3TigerScreamTimer = 0.0;
bool l3TigerScreamPlayed = false;
bool l3TigerAttacking = false;
bool l3DoorAudioPlayed = false;
const double L3_TIGER_SPEED = 6.0;
const double L3_TIGER_ATTACK_RANGE = 95.0;
const double L3_TIGER_DAMAGE_COOLDOWN = 0.45;

void startLevel3TigerTrap()
{
    state = LEVEL3_TIGER;
    // The player enters this atrium from the left, while the tiger appears
    // from the right and runs toward the player.
    assassin.x = 80.0;
    assassin.y = GROUND_Y;
    l3TigerX = WIN_W + 130.0;
    l3TigerAnimTimer = 0.0;
    l3TigerAnimFrame = 0;
    l3TigerAttackTimer = 0.0;
    l3TigerFlashTimer = 0.22;
    l3TigerScreamTimer = 1.0;
    l3TigerScreamPlayed = false;
    l3TigerAttacking = false;
    assassin.dx = 0.0;
    assassin.facingRight = true;
    assassin.walkFrame = 0;
    assassin.walkFrameTimer = 0.0;
    movementAKeyDown = false;
    movementDKeyDown = false;
    playGameAudio("TIGER_ROAR.mp3");
}

void updateLevel3Tiger()
{
    bool leftHeld = movementAKeyDown || isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = movementDKeyDown || isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

    // Use the same movement path as the other Level 3 rooms. Keep dx set
    // every update so A/D input also drives facing and running animation.
    if (leftHeld && !rightHeld)
        assassin.dx = -L3_WALK_SPEED;
    else if (rightHeld && !leftHeld)
        assassin.dx = L3_WALK_SPEED;
    else
        assassin.dx = 0.0;

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 80.0, WIN_W - 80.0);

    if (l3TigerX < assassin.x) l3TigerX += L3_TIGER_SPEED;
    else l3TigerX -= L3_TIGER_SPEED;

    l3TigerAnimTimer += TICK;
    if (l3TigerAnimTimer >= 0.08)
    {
        l3TigerAnimTimer = 0.0;
        l3TigerAnimFrame = (l3TigerAnimFrame + 1) % L3_TIGER_RUN_FRAME_COUNT;
    }

    if (fabs(l3TigerX - assassin.x) <= L3_TIGER_ATTACK_RANGE)
    {
        if (!l3TigerAttacking)
        {
            l3TigerScreamTimer = 1.0;
            l3TigerScreamPlayed = false;
        }
        l3TigerAttacking = true;
        l3TigerAttackTimer += TICK;
        if (l3TigerAttackTimer >= L3_TIGER_DAMAGE_COOLDOWN)
        {
            l3TigerAttackTimer = 0.0;
            playerHP -= 80;
            l3TigerFlashTimer = 0.18;
            if (playerHP <= 0)
            {
                playerHP = 0;
                state = GAME_OVER;
                return;
            }
        }
    }
    else
    {
        l3TigerAttacking = false;
        l3TigerAttackTimer = 0.0;
    }

    if (l3TigerFlashTimer > 0.0)
    {
        l3TigerFlashTimer -= TICK;
        if (l3TigerFlashTimer < 0.0) l3TigerFlashTimer = 0.0;
    }
    if (l3TigerAttacking && !l3TigerScreamPlayed)
    {
        l3TigerScreamTimer -= TICK;
        if (l3TigerScreamTimer <= 0.0)
        {
            playGameAudio("SCREAM_AFTER_TIGER_ATTACK.mp3");
            l3TigerScreamPlayed = true;
        }
    }
    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();
}

void skipLevel3VaultIntro()
{
    state = LEVEL3_VAULT;
    assassin.x = 80.0;
    isHiding = false;
    vaultIntroShot = 0;
    isVaultOpen = false; // Reset the door to closed when you enter the room
    l3CompletionAudioPlayed = false;
}

void updateLevel3VaultIntro()
{
    vaultIntroTimer += TICK;
    if (vaultIntroTimer >= VAULT_SHOT_TIME)
    {
        vaultIntroTimer = 0.0;
        vaultIntroShot++;
        if (vaultIntroShot >= 2) 
        {
            skipLevel3VaultIntro();
        }
    }
}


void resetCeilingHang();
void updateLevel3Transition()
{
    l3TransitionTimer += TICK;
    if (l3TransitionTimer >= 2.0) 
    {
        // The 3_7 bomb countdown must not carry into room 3_8.
        stopGameAudio();
        stopGameAudioChannel("door_scene_audio");
        l3DoorSceneAudioPlaying = false;
        l3BombActive = false;
        l3BombTimer = 0.0;
        state = LEVEL3_INTERIOR_2;
        assassin.x = 80.0; 
        assassin.y = GROUND_Y;
        resetCeilingHang();
        
        isHiding = false;
        hasSecurityKey = false; 
        isKeyDropped = false;
        kKeyWasHeld = false;
        
        resetNinjas();
        ambushTriggered = false;
        roomGuard.active = false;   
        roomGuard.alerted = false;
        roomGuard.retreating = false;
        roomGuard.isDead = false;   
        roomGuard.delayTimer = 4.0; 
        roomGuard.x = WIN_W + 150.0; 
        roomGuard.y = GROUND_Y;
        roomGuard.animFrame = 0;
        roomGuard.animTimer = 0.0;
        l3DoorChoiceActive = false;
        l3WrongDoorMessage = false;
        l3DoorAudioPlayed = false;
    }
}

// ---------------------------------------------------------
// ROOM 3_8 CEILING-HANG LOGIC
// ---------------------------------------------------------
enum CeilingHangState { CEILING_GROUND, CEILING_CLIMBING_UP, CEILING_HANGING, CEILING_CLIMBING_DOWN };
CeilingHangState ceilingHangState = CEILING_GROUND;
double ceilingHangTimer = 0.0;
bool ceilingEKeyWasHeld = false;
int ceilingHookIndex = 0;
bool ceilingAttackActive = false;
bool ceilingAttackResolved = false;
double ceilingAttackTimer = 0.0;
const int L3_SPIDER_COUNT = 4;
bool l3SpiderSpawned = false;
bool l3SpiderSpawnPending = false;
bool l3SpiderAudioPlaying = false;
bool l3GuardAudioPlaying = false;
bool l3ClimbingAudioPlaying = false;
bool l3SpiderActive[L3_SPIDER_COUNT] = { false, false, false, false };
double l3SpiderX[L3_SPIDER_COUNT] = { -100.0, -220.0, -340.0, -460.0 };
double l3SpiderAnimTimer[L3_SPIDER_COUNT] = { 0.0, 0.0, 0.0, 0.0 };
int l3SpiderAnimFrame[L3_SPIDER_COUNT] = { 0, 0, 0, 0 };
double l3SpiderDamageAccumulator[L3_SPIDER_COUNT] = { 0.0, 0.0, 0.0, 0.0 };
const double L3_SPIDER_SPEED = 4.0;
const double L3_SPIDER_DAMAGE_PER_SECOND = 30.0;
const double L3_SPIDER_DAMAGE_RANGE = 75.0;
const double CEILING_HOOK_X[2] = { 285.0, 555.0 };
const double CEILING_HOOK_Y = WIN_H - 105.0;
const double CEILING_HANG_Y = WIN_H - 145.0;
const double CEILING_HANG_RANGE = 70.0;
const double CEILING_CLIMB_TIME = 0.32;

bool isCeilingHanging()
{
    return ceilingHangState == CEILING_HANGING;
}

void resetCeilingHang()
{
    ceilingHangState = CEILING_GROUND;
    ceilingHangTimer = 0.0;
    ceilingEKeyWasHeld = false;
    ceilingHookIndex = 0;
    ceilingAttackActive = false;
    ceilingAttackResolved = false;
    ceilingAttackTimer = 0.0;
    l3SpiderSpawned = false;
    l3SpiderSpawnPending = false;
    l3SpiderAudioPlaying = false;
    stopGameAudioChannel("spider_audio");
    l3GuardAudioPlaying = false;
    stopGameAudioChannel("guard_audio");
    l3ClimbingAudioPlaying = false;
    stopGameAudioChannel("climb_audio");

    for (int i = 0; i < L3_SPIDER_COUNT; i++)
    {
        l3SpiderActive[i] = false;
        l3SpiderX[i] = -100.0 - i * 120.0;
        l3SpiderAnimTimer[i] = 0.0;
        l3SpiderAnimFrame[i] = 0;
        l3SpiderDamageAccumulator[i] = 0.0;
    }
}

void startCeilingAttack()
{
    if (ceilingHangState != CEILING_HANGING || ceilingAttackActive) return;
    playGameAudio("KATANA_SOUND.mp3");
    ceilingAttackActive = true;
    ceilingAttackResolved = false;
    ceilingAttackTimer = 0.0;
    isKatanaAttacking = false;
}

// ---------------------------------------------------------
// ROOM 3_8 STEALTH LOGIC
// ---------------------------------------------------------
void updateLevel3Interior2()
{
    bool leftHeld  = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);
    bool downHeld  = isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN);
    bool kHeld     = isKeyPressed('k') || isKeyPressed('K');
    bool eHeld     = isKeyPressed('e') || isKeyPressed('E');

    if (l3DoorChoiceActive)
    {
        if (hasSecurityKey && kHeld)
        {
            int selectedDoor = (fabs(assassin.x - L3_DOOR_LEFT_X) <= 130.0) ? 0 :
                               ((fabs(assassin.x - L3_DOOR_RIGHT_X) <= 130.0) ? 1 : -1);

            if (selectedDoor >= 0)
            {
                if (selectedDoor == l3CorrectDoor)
                {
                    stopGameAudioChannel("door_scene_audio");
                    l3DoorSceneAudioPlaying = false;
                    l3DoorChoiceActive = false;
                    l3WrongDoorMessage = false;
                    state = LEVEL3_VAULT_INTRO;
                    vaultIntroTimer = 0.0;
                    vaultIntroShot = 0;
                }
                else
                {
                    l3WrongDoorMessage = true;
                    stopGameAudioChannel("door_scene_audio");
                    l3DoorSceneAudioPlaying = false;
                    playGameAudio("WARNING.mp3");
                    startLevel3TigerTrap();
                }
            }
        }

        if (leftHeld && !rightHeld)
        {
            assassin.dx = -L3_WALK_SPEED;
            assassin.x += assassin.dx;
        }
        else if (rightHeld && !leftHeld)
        {
            assassin.dx = L3_WALK_SPEED;
            assassin.x += assassin.dx;
        }
        else
        {
            assassin.dx = 0.0;
        }
        assassin.x = clampD(assassin.x, 150.0, WIN_W - 150.0);

        // Keep the normal katana animation active in the door scene. The
        // combat update below is skipped while choosing a door, so advance
        // the attack frames here without applying damage to anything.
        if (isKatanaAttacking)
        {
            katanaAttackTimer += TICK;
            if (katanaAttackTimer > 0.06)
            {
                katanaAttackTimer = 0.0;
                katanaAttackFrame++;
                if (katanaAttackFrame >= 5)
                {
                    if (isKatanaHeld)
                        katanaAttackFrame = 0;
                    else
                    {
                        isKatanaAttacking = false;
                        katanaAttackFrame = 0;
                    }
                }
            }
        }

        updatePlayerFacing(leftHeld, rightHeld);
        updatePlayerWalkAnimation();
        return;
    }

    if (ceilingAttackActive)
    {
        ceilingAttackTimer += TICK;

        // Resolve the sword strike near the middle of the fast attack.
        if (!ceilingAttackResolved && ceilingAttackTimer >= 0.18)
        {
            ceilingAttackResolved = true;
            bool directHeadHit = roomGuard.active && !roomGuard.isDead &&
                                 fabs(assassin.x - roomGuard.x) <= 42.0;

            if (directHeadHit)
            {
                roomGuard.isDead = true;
                playGameAudio("SCREAM_ON_DEATH_ENEMY_OR_PLAYER.mp3");
                roomGuard.alerted = false;
                roomGuard.retreating = false;
                isKeyDropped = true;
                keyX = roomGuard.x - 20.0;
                keyY = GROUND_Y;
                playerEP += 20;
            }
            else if (roomGuard.active && !roomGuard.isDead)
            {
                // A missed ceiling strike exposes the player. The existing
                // retreat logic then sends the guard away and spawns the
                // same two-enemy ambush as before.
                roomGuard.alerted = true;
                roomGuard.retreating = true;
            }
        }

        if (ceilingAttackTimer >= 0.65)
            ceilingAttackActive = false;
    }

    if (eHeld && !ceilingEKeyWasHeld && ceilingHangState != CEILING_CLIMBING_UP && ceilingHangState != CEILING_CLIMBING_DOWN)
    {
        if (ceilingHangState == CEILING_GROUND)
        {
            double nearest = fabs(assassin.x - CEILING_HOOK_X[0]);
            ceilingHookIndex = 0;
            if (fabs(assassin.x - CEILING_HOOK_X[1]) < nearest) ceilingHookIndex = 1;

            if (fabs(assassin.x - CEILING_HOOK_X[ceilingHookIndex]) <= CEILING_HANG_RANGE)
            {
                assassin.x = CEILING_HOOK_X[ceilingHookIndex];
                assassin.dx = 0.0;
                ceilingHangTimer = 0.0;
                ceilingHangState = CEILING_CLIMBING_UP;
                isKatanaAttacking = false;
                playGameAudioChannel("CLIMBING_ROOF.mp3", "climb_audio", true);
                l3ClimbingAudioPlaying = true;
            }
        }
        else if (ceilingHangState == CEILING_HANGING)
        {
            ceilingHangTimer = 0.0;
            ceilingHangState = CEILING_CLIMBING_DOWN;
            playGameAudioChannel("CLIMBING_ROOF.mp3", "climb_audio", true);
            l3ClimbingAudioPlaying = true;
        }
    }
    ceilingEKeyWasHeld = eHeld;

    if (ceilingHangState == CEILING_CLIMBING_UP || ceilingHangState == CEILING_CLIMBING_DOWN)
    {
        ceilingHangTimer += TICK;
        if (ceilingHangTimer >= CEILING_CLIMB_TIME)
        {
            ceilingHangTimer = CEILING_CLIMB_TIME;
            ceilingHangState = (ceilingHangState == CEILING_CLIMBING_UP) ? CEILING_HANGING : CEILING_GROUND;
            if (ceilingHangState == CEILING_GROUND) assassin.y = GROUND_Y;
            if (l3ClimbingAudioPlaying)
            {
                stopGameAudioChannel("climb_audio");
                l3ClimbingAudioPlaying = false;
            }
        }
    }

    if (ceilingHangState != CEILING_GROUND)
    {
        assassin.dx = 0.0;
        isHiding = false;
    }

    bool inHidingZone = (assassin.x > 480.0 && assassin.x < 720.0);

    if (!roomGuard.active && !roomGuard.alerted && roomGuard.delayTimer > 0.0 && !ambushTriggered && !roomGuard.isDead)
    {
        roomGuard.delayTimer -= TICK;
        if (roomGuard.delayTimer <= 0.0) roomGuard.active = true; 
    }

    if (roomGuard.active && !roomGuard.isDead) 
    {
        if (!roomGuard.retreating)
        {
            roomGuard.x -= L3_GUARD_SPEED; 
            roomGuard.animTimer += TICK;
            if (roomGuard.animTimer >= 0.15)
            {
                roomGuard.animTimer = 0.0;
                roomGuard.animFrame = (roomGuard.animFrame + 1) % GUARD_FRAME_COUNT;
            }

            if (roomGuard.x < WIN_W - 60.0 && roomGuard.x > 0 && !isHiding && !isCeilingHanging() && !ceilingAttackActive && !ambushTriggered)
            {
                if (assassin.x < roomGuard.x + 30.0) 
                {
                    roomGuard.alerted = true;
                    roomGuard.retreating = true; 
                }
            }

            if (roomGuard.x < -150.0) roomGuard.active = false;
        }
        else 
        {
            roomGuard.x += L3_GUARD_SPEED * 1.8; 
            roomGuard.animTimer += TICK;
            if (roomGuard.animTimer >= 0.1) 
            {
                roomGuard.animTimer = 0.0;
                roomGuard.animFrame = (roomGuard.animFrame + 1) % GUARD_FRAME_COUNT;
            }

            if (roomGuard.x > WIN_W + 150.0)
            {
                roomGuard.active = false;
                ambushTriggered = true;
                isHiding = false; 
                playGameAudio("AMBUSH_TRIGGERED.mp3");
                spawnNinja(RED_NINJA, WIN_W + 100.0);
                spawnNinja(BLUE_NINJA, WIN_W + 250.0);
            }
        }
    }

    if (roomGuard.active && !roomGuard.isDead)
    {
        if (!l3GuardAudioPlaying)
        {
            playGameAudioChannel("GUARD_WALKING_SOUND.mp3", "guard_audio", true);
            l3GuardAudioPlaying = true;
        }
    }
    else if (l3GuardAudioPlaying)
    {
        stopGameAudioChannel("guard_audio");
        l3GuardAudioPlaying = false;
    }

    if (ceilingHangState == CEILING_HANGING)
    {
        assassin.dx = 0.0;
    }
    // Spawn spiders only after the guard has been killed. If the player is
    // hanging or climbing, wait until the player returns to the ground.
    if (!l3SpiderSpawned && roomGuard.isDead)
    {
        if (ceilingHangState == CEILING_GROUND)
        {
            l3SpiderSpawned = true;
            l3SpiderSpawnPending = false;
            ambushTriggered = true;
            spawnNinja(RED_NINJA, WIN_W + 100.0);
            spawnNinja(BLUE_NINJA, WIN_W + 250.0);

            for (int i = 0; i < L3_SPIDER_COUNT; i++)
            {
                l3SpiderActive[i] = true;
                l3SpiderX[i] = -100.0 - i * 120.0;
                l3SpiderAnimTimer[i] = 0.0;
                l3SpiderAnimFrame[i] = 0;
                l3SpiderDamageAccumulator[i] = 0.0;
            }
        }
        else
        {
            l3SpiderSpawnPending = true;
        }
    }

    // Spawn queued spiders when the player returns to the ground.
    if (l3SpiderSpawnPending &&
        !l3SpiderSpawned &&
        ceilingHangState == CEILING_GROUND)
    {
        l3SpiderSpawned = true;
        l3SpiderSpawnPending = false;
        ambushTriggered = true;
        spawnNinja(RED_NINJA, WIN_W + 100.0);
        spawnNinja(BLUE_NINJA, WIN_W + 250.0);

        for (int i = 0; i < L3_SPIDER_COUNT; i++)
        {
            l3SpiderActive[i] = true;
            l3SpiderX[i] = -100.0 - i * 120.0;
            l3SpiderAnimTimer[i] = 0.0;
            l3SpiderAnimFrame[i] = 0;
            l3SpiderDamageAccumulator[i] = 0.0;
        }
    }

    bool ninjaThreatAlive = false;
    for (int i = 0; i < MAX_NINJAS; i++)
    {
        if (ninjas[i].active && ninjas[i].state != NINJA_DEAD)
        {
            ninjaThreatAlive = true;
            break;
        }
    }

    bool anySpiderMoving = false;
    for (int i = 0; i < L3_SPIDER_COUNT; i++)
    {
        if (!l3SpiderActive[i]) continue;

        // During the red/blue ambush, spiders hunt the enemies first. Once
        // both enemies are dead, they stop and wait for the player to land.
        bool spiderShouldMove = !ambushTriggered || ninjaThreatAlive;
        if (spiderShouldMove) anySpiderMoving = true;
        if (spiderShouldMove)
            l3SpiderX[i] += L3_SPIDER_SPEED;
        l3SpiderAnimTimer[i] += TICK;
        if (l3SpiderAnimTimer[i] >= 0.10)
        {
            l3SpiderAnimTimer[i] = 0.0;
            l3SpiderAnimFrame[i] = (l3SpiderAnimFrame[i] + 1) % 3;
        }

        if (l3SpiderX[i] > WIN_W + 110.0 && spiderShouldMove)
            l3SpiderActive[i] = false;

        // A spider kills a red or blue ninja on contact.
        for (int n = 0; n < MAX_NINJAS; n++)
        {
            if (l3SpiderActive[i] && ninjas[n].active && ninjas[n].state != NINJA_DEAD &&
                fabs(l3SpiderX[i] - ninjas[n].x) <= 70.0)
            {
                ninjas[n].hp = 0;
                ninjas[n].state = NINJA_DEAD;
                playGameAudio("SCREAM_ON_DEATH_ENEMY_OR_PLAYER.mp3");
                playerEP += 20;
            }
        }

    if (anySpiderMoving && !l3SpiderAudioPlaying)
    {
        playGameAudioChannel("SPIDER_WALKING.mp3", "spider_audio", true);
        l3SpiderAudioPlaying = true;
    }
    else if (l3SpiderAudioPlaying && !anySpiderMoving)
    {
        stopGameAudioChannel("spider_audio");
        l3SpiderAudioPlaying = false;
    }

        bool spiderCanReachPlayer = fabs(l3SpiderX[i] - assassin.x) <= L3_SPIDER_DAMAGE_RANGE;
        if (spiderCanReachPlayer && !isHiding && ceilingHangState == CEILING_GROUND)
        {
            l3SpiderDamageAccumulator[i] += L3_SPIDER_DAMAGE_PER_SECOND * TICK;
            while (l3SpiderDamageAccumulator[i] >= 1.0)
            {
                playerHP--;
                l3SpiderDamageAccumulator[i] -= 1.0;
            }
            if (playerHP <= 0)
            {
                playerHP = 0;
                state = GAME_OVER;
                return;
            }
        }
    }

    if (inHidingZone && downHeld && !ambushTriggered)
    {
        isHiding = true;
        assassin.dx = 0.0; 
    }
    else
    {
        isHiding = false;
        if (isKatanaAttacking) {
            if (assassin.facingRight) assassin.dx = L3_WALK_SPEED * 0.4;
            else assassin.dx = -L3_WALK_SPEED * 0.4;
        } 
        else if (leftHeld && !rightHeld) assassin.dx = -L3_WALK_SPEED;
        else if (rightHeld && !leftHeld) assassin.dx = L3_WALK_SPEED;
        else assassin.dx = 0.0;
    }

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 60.0, WIN_W - 60.0);   
    
    if (!isHiding && ceilingHangState != CEILING_HANGING) 
    {
        updatePlayerFacing(leftHeld, rightHeld);
        updatePlayerWalkAnimation();
    }

    if (isKeyDropped && !hasSecurityKey)
    {
        if (fabs(assassin.x - keyX) < 60.0)
        {
            if (kHeld && !kKeyWasHeld)
            {
                hasSecurityKey = true;
                isKeyDropped = false;
            }
        }
    }
    kKeyWasHeld = kHeld;

    // After the 3_8 guard, ninjas, and spiders are defeated, reaching the
    // right edge opens the two-door scene. The key is still required later
    // to enter a door, but it must not prevent the scene from appearing.
    if (assassin.x >= WIN_W - 70.0)
    {
        bool allDead = true;
        for (int i = 0; i < MAX_NINJAS; i++)
        {
            if (ninjas[i].active && ninjas[i].state != NINJA_DEAD)
            {
                allDead = false;
                break;
            }
        }

        bool allSpidersDead = true;
        for (int i = 0; i < L3_SPIDER_COUNT; i++)
        {
            if (l3SpiderActive[i])
            {
                allSpidersDead = false;
                break;
            }
        }

        if (allDead && allSpidersDead)
        {
            l3CorrectDoor = rand() % 2;
            l3DoorChoiceActive = true;
            l3WrongDoorMessage = false;
            if (!l3DoorSceneAudioPlaying)
            {
                playGameAudioChannel("3_8_1 SOUND.mp3", "door_scene_audio", true);
                l3DoorSceneAudioPlaying = true;
            }
            assassin.x = WIN_W / 2.0;
            assassin.y = GROUND_Y;
        }
    }

    if (isKatanaAttacking && !isHiding)
    {
        katanaAttackTimer += TICK;
        if (katanaAttackTimer > 0.06) 
        {
            katanaAttackTimer = 0.0;
            katanaAttackFrame++;
            
            if (katanaAttackFrame == 2 && !hasPlayerDealtDamageThisSwing)
            {
                // Each Level 3 spider dies from one successful katana hit.
                for (int i = 0; i < L3_SPIDER_COUNT; i++)
                {
                    if (l3SpiderActive[i] && fabs(assassin.x - l3SpiderX[i]) < 120.0)
                    {
                        l3SpiderActive[i] = false;
                        hasPlayerDealtDamageThisSwing = true;
                        playerEP += 20;
                        break;
                    }
                }

                if (roomGuard.active && !roomGuard.alerted && !roomGuard.retreating && !roomGuard.isDead)
                {
                    if (assassin.x > roomGuard.x - 40.0 && fabs(assassin.x - roomGuard.x) < 120.0)
                    {
                        roomGuard.isDead = true;      
                        playGameAudio("SCREAM_ON_DEATH_ENEMY_OR_PLAYER.mp3");
                        isKeyDropped = true;          
                        keyX = roomGuard.x - 20.0;    
                        keyY = GROUND_Y;              
                        hasPlayerDealtDamageThisSwing = true;
                        playerEP += 20; // NEW: Grant EP on kill
                    }
                }

                if (!hasPlayerDealtDamageThisSwing)
                {
                    for (int i = 0; i < MAX_NINJAS; i++)
                    {
                        if (ninjas[i].active && ninjas[i].state != NINJA_DEAD)
                        {
                            if (fabs(assassin.x - ninjas[i].x) < 120.0)
                            {
                                ninjas[i].hp -= 50; 
                                hasPlayerDealtDamageThisSwing = true;
                                if (ninjas[i].hp <= 0) 
                                {
                                    ninjas[i].state = NINJA_DEAD;
                                    playGameAudio("SCREAM_ON_DEATH_ENEMY_OR_PLAYER.mp3");
                                    playerEP += 20; // NEW: Grant EP on kill
                                }
                                break; 
                            }
                        }
                    }
                }
            }

            if (katanaAttackFrame >= 5) 
            {
                if (isKatanaHeld && !isHiding) {
                    katanaAttackFrame = 0; 
                    hasPlayerDealtDamageThisSwing = false;
                } else {
                    isKatanaAttacking = false;
                    katanaAttackFrame = 0;
                    hasPlayerDealtDamageThisSwing = false;
                }
            }
        }
    }

    for (int i = 0; i < MAX_NINJAS; i++)
    {
        if (!ninjas[i].active || ninjas[i].state == NINJA_DEAD) continue; 

        if (ninjas[i].state == NINJA_RUNNING)
        {
            if (ninjas[i].x < assassin.x) ninjas[i].x += L3_NINJA_SPEED;
            else ninjas[i].x -= L3_NINJA_SPEED; 

            ninjas[i].animTimer += TICK;
            if (ninjas[i].animTimer > 0.08)
            {
                ninjas[i].animTimer = 0.0;
                ninjas[i].animFrame = (ninjas[i].animFrame + 1) % NINJA_RUN_FRAME_COUNT;
            }

            if (fabs(ninjas[i].x - assassin.x) < 90.0)
            {
                ninjas[i].state = NINJA_ATTACKING;
                ninjas[i].animFrame = 0;
                ninjas[i].animTimer = 0.0;
                ninjas[i].hasDealtDamageThisSwing = false;
            }
        }
        else if (ninjas[i].state == NINJA_ATTACKING)
        {
            ninjas[i].animTimer += TICK;
            if (ninjas[i].animTimer > 0.1) 
            {
                ninjas[i].animTimer = 0.0;
                ninjas[i].animFrame++;

                if (ninjas[i].animFrame == 2 && !ninjas[i].hasDealtDamageThisSwing)
                {
                    playerHP -= 200; 
                    ninjas[i].hasDealtDamageThisSwing = true;
                    if (playerHP <= 0) { playerHP = 0; state = GAME_OVER; }
                }

                if (ninjas[i].animFrame >= NINJA_ATTACK_FRAME_COUNT)
                {
                    if (fabs(ninjas[i].x - assassin.x) < 90.0)
                    {
                        ninjas[i].animFrame = 0;
                        ninjas[i].hasDealtDamageThisSwing = false;
                    }
                    else
                    {
                        ninjas[i].state = NINJA_RUNNING;
                        ninjas[i].animFrame = 0;
                    }
                }
            }
        }
    }
}


// ---------------------------------------------------------
// ROOM 3_11 (FINAL VAULT ROOM GAMEPLAY LOGIC)
// ---------------------------------------------------------
void updateLevel3Vault()
{
    bool leftHeld  = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);
    bool xHeld     = isKeyPressed('x') || isKeyPressed('X'); 

    if (isKatanaAttacking) {
        if (assassin.facingRight) assassin.dx = L3_WALK_SPEED * 0.4;
        else assassin.dx = -L3_WALK_SPEED * 0.4;
    } 
    else if (leftHeld && !rightHeld) assassin.dx = -L3_WALK_SPEED;
    else if (rightHeld && !leftHeld) assassin.dx = L3_WALK_SPEED;
    else assassin.dx = 0.0;

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 60.0, WIN_W - 60.0);   
    updatePlayerFacing(leftHeld, rightHeld);
    updatePlayerWalkAnimation();

    if (!isVaultOpen && hasSecurityKey && assassin.x > 300.0 && assassin.x < 500.0)
    {
        if (xHeld && !xKeyWasHeld)
        {
            isVaultOpen = true; 
            if (!l3CompletionAudioPlayed)
            {
                playGameAudio("LEVEL_COMPLETED.mp3");
                l3CompletionAudioPlayed = true;
            }
        }
    }
    xKeyWasHeld = xHeld;

    if (isKatanaAttacking)
    {
        katanaAttackTimer += TICK;
        if (katanaAttackTimer > 0.06) 
        {
            katanaAttackTimer = 0.0;
            katanaAttackFrame++;
            if (katanaAttackFrame >= 5) 
            {
                if (isKatanaHeld && !isHiding) {
                    katanaAttackFrame = 0; // Loop animation
                } else {
                    isKatanaAttacking = false;
                    katanaAttackFrame = 0;
                }
            }
        }
    }
}

#endif