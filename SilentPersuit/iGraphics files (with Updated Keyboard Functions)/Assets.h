#ifndef ASSETS_H
#define ASSETS_H

// =========================================================
// SILENT PURSUIT - Assets.h
// =========================================================

#include "Config.h"
#include <stdio.h>

// ---------------------------------------------------------
// 1. BACKGROUND IMAGES
// ---------------------------------------------------------
unsigned int loadingBgTexture = 0;
unsigned int menuBgTexture = 0;
unsigned int level1BgTexture = 0;
unsigned int level1Scene2BgTexture = 0;
unsigned int level1Scene3BgTexture = 0;
unsigned int buildingFloorTextures[BUILDING_TILE_COUNT] = { 0 };
unsigned int smokeCloudTexture = 0;
unsigned int keyTexture = 0;
unsigned int mannequinTexture = 0;

const int LEVEL3_INTRO_SHOT_COUNT = 3;
unsigned int level3IntroTexture[LEVEL3_INTRO_SHOT_COUNT] = { 0, 0, 0 };

unsigned int level3StartBgTexture = 0;
unsigned int level3BlastTexture[2] = { 0, 0 };
unsigned int level3AftermathTexture = 0;
unsigned int level3InteriorTexture = 0;
unsigned int level3Interior2Texture = 0;
unsigned int level3DoorChoiceTexture = 0;
unsigned int level3TigerAtriumTexture = 0;
const int L3_TIGER_RUN_FRAME_COUNT = 5;
unsigned int l3TigerRunFrames[L3_TIGER_RUN_FRAME_COUNT] = { 0, 0, 0, 0, 0 };
unsigned int l3TigerAttackTexture = 0;

// Vault Textures (4 images, including the open door!)
unsigned int level3VaultTex[4] = { 0, 0, 0, 0 };

// ---------------------------------------------------------
// 1b. LEVEL 4 - IMPERIAL PALACE ROOM BACKGROUNDS
// ---------------------------------------------------------
// 6 areas, each with either 2 or 3 shots, walked through in this
// order: grand atrium, colonnade, treasury, secret archives,
// armory, imperial throne room. See level4RoomSequence in
// Level4.h for the exact order (it's just this enum in order).
// The Execution Dais area has been removed - the final boss
// fight now happens in the Imperial Throne room instead (see
// Level4.h and Screens.h).
enum Level4RoomId
{
    L4_GRAND_ATRIUM_1 = 0, L4_GRAND_ATRIUM_2, L4_GRAND_ATRIUM_3,
    L4_COLONADE_1, L4_COLONADE_2, L4_COLONADE_3,
    L4_TREASURY_1, L4_TREASURY_2, L4_TREASURY_3,
    L4_SECRET_ARCHIVES_1, L4_SECRET_ARCHIVES_2, L4_SECRET_ARCHIVES_3,
    L4_ARMORY_1, L4_ARMORY_2,                          // only 2 shots
    L4_IMPERIAL_THRONE_1, L4_IMPERIAL_THRONE_2, L4_IMPERIAL_THRONE_3,
    LEVEL4_ROOM_COUNT
};
unsigned int level4RoomTex[LEVEL4_ROOM_COUNT] = { 0 };
unsigned int level4StairIntroTexture = 0;

// ---------------------------------------------------------
// 1c. LEVEL 4 - ENEMY SPRITES
// ---------------------------------------------------------
// The entrance rooms (grand atrium / colonade) reuse the ninja
// sprites in section 5 below, and the armory reuses the g-01/
// g-02/g-03/g-dead guard sprites from section 5 as well - no
// separate art needed for either. maid/hallway/spider each cycle
// through 4 walk frames continuously (including while attacking -
// no separate attack pose), all flat inside Images/Enemy/ using
// this exact naming:
//   maidenemy_1.png     maidenemy_2.png     maidenemy_3.png     maidenemy_4.png
//   hallwayenemy_1.png  hallwayenemy_2.png  hallwayenemy_3.png  hallwayenemy_4.png
//   libraryspider_1.png libraryspider_2.png libraryspider_3.png libraryspider_4.png
// The boss has its own set - see below.
const int L4_ENEMY_WALK_FRAME_COUNT = 4;

// The boss sheet only has 7 actual images (bossenemy_1..7). Using
// all of them as one continuous walk cycle made it flash through
// a dynamic swing pose and a collapsing pose just from walking
// around, which read as randomly "dramatic". Now: 2 calm frames
// for walking, 1 for the attack swing, 1 for staggering/
// recovering right after a swing, 1 for the final defeated pose.
// bossenemy_3 and bossenemy_6 are loaded but not used yet -
// reserved for a 2nd attack pose or a mid-collapse frame later.
const int BOSS_WALK_FRAME_COUNT = 2;
unsigned int bossWalkFrames[BOSS_WALK_FRAME_COUNT] = { 0, 0 };   // bossenemy_1, bossenemy_2
unsigned int bossAttackTex = 0;    // bossenemy_4 - shown only during the attack swing
unsigned int bossRecoverTex = 0;   // bossenemy_5 - shown only while staggering after a swing
unsigned int bossReserveTex = 0;   // bossenemy_6 - loaded but not used yet
unsigned int bossDeadTex = 0;      // bossenemy_7 - shown once the boss is defeated

unsigned int maidWalkFrames[L4_ENEMY_WALK_FRAME_COUNT] = { 0, 0, 0, 0 };
unsigned int spiderWalkFrames[L4_ENEMY_WALK_FRAME_COUNT] = { 0, 0, 0, 0 };

// Hallway enemy - 7 run frames (hallway_1..7.png) + a dedicated
// dissolve/death frame (hallway_8.png), shown once it dies.
const int HALLWAY_FOE_FRAME_COUNT = 7;
unsigned int hallwayWalkFrames[HALLWAY_FOE_FRAME_COUNT] = { 0, 0, 0, 0, 0, 0, 0 };
unsigned int hallwayDeadTex = 0;

unsigned int rpgPropTexture = 0;
const int RPG_LIFT_FRAME_COUNT = 3;
unsigned int rpgLiftFrames[RPG_LIFT_FRAME_COUNT] = { 0, 0, 0 };
unsigned int rpgStandTexture = 0;

const int RPG_LIFT_DRAW_W[RPG_LIFT_FRAME_COUNT] = { 206, 206, 206 };
const int RPG_LIFT_DRAW_H[RPG_LIFT_FRAME_COUNT] = { 360, 360, 360 };
const int RPG_LIFT_PAD_BOTTOM[RPG_LIFT_FRAME_COUNT] = { 28,  27,  10 };
const int RPG_LIFT_HIP_OFFSET_X[RPG_LIFT_FRAME_COUNT] = { 106, 123, 109 };

const int RPG_STAND_DRAW_W = 206;
const int RPG_STAND_DRAW_H = 360;
const int RPG_STAND_PAD_BOTTOM = 14;
const int RPG_STAND_HIP_OFFSET_X = 129;
const int RPG_PROP_DRAW_W = 225;
const int RPG_PROP_DRAW_H = 41;

// ---------------------------------------------------------
// 2. HOW BIG SPRITES ARE DRAWN ON SCREEN
// ---------------------------------------------------------
const int SPRITE_DRAW_W = 140;
const int SPRITE_DRAW_H = 260;
const double SPRITE_Y_OFFSET = 8.0;

const int IDLE_DRAW_W = 162;
const int IDLE_DRAW_H = 300;
const double IDLE_Y_OFFSET = -22.0;

const int L3_SPRITE_DRAW_W = 105;
const int L3_SPRITE_DRAW_H = 195;
const double L3_SPRITE_Y_OFFSET = 6.0;

const int L3_IDLE_DRAW_W = 121;
const int L3_IDLE_DRAW_H = 225;
const double L3_IDLE_Y_OFFSET = -16.5;

const int KATANA_DRAW_W = 200;
const int KATANA_DRAW_H = 260;

const int KATANA_RUN_DRAW_W = 180;
const int KATANA_RUN_DRAW_H = 240;
const double KATANA_RUN_Y_OFFSET = 8.0;

const int KATANA_IDLE_DRAW_W = 220;
const int KATANA_IDLE_DRAW_H = 286;
const double KATANA_IDLE_Y_OFFSET = 0.0;

// ---------------------------------------------------------
// LEVEL 4 - UPGRADED KNIFE (player weapon, replaces the katana
// in Level 4 - more powerful, see L4_KNIFE_SWING_DAMAGE in
// Level4.h). Unlike every other weapon in the game, this one
// has dedicated left-facing and right-facing art instead of a
// single direction that gets mirrored - see pickPlayerSprite()
// in Player.h. File layout, all flat in Images/Knife/:
//   idle.png
//   walk_right_1.png..walk_right_6.png
//   walk_left_1.png..walk_left_6.png
//   attack_right_1.png..attack_right_4.png
//   attack_left_1.png..attack_left_4.png
// ---------------------------------------------------------
unsigned int knifeIdleTex = 0;
unsigned int tornPlayerTextures[3] = { 0, 0, 0 };
unsigned int ceilingHangTexture = 0;
unsigned int ceilingAttackTexture = 0;
unsigned int l3SpiderFrames[3] = { 0, 0, 0 };

const int KNIFE_WALK_FRAME_COUNT = 6;
unsigned int knifeWalkRightFrames[KNIFE_WALK_FRAME_COUNT];
unsigned int knifeWalkLeftFrames[KNIFE_WALK_FRAME_COUNT];

const int KNIFE_ATTACK_FRAME_COUNT = 4;
unsigned int knifeAttackRightFrames[KNIFE_ATTACK_FRAME_COUNT];
unsigned int knifeAttackLeftFrames[KNIFE_ATTACK_FRAME_COUNT];

const int KNIFE_DRAW_W = 200, KNIFE_DRAW_H = 260;
const int KNIFE_RUN_DRAW_W = 180, KNIFE_RUN_DRAW_H = 240;
const double KNIFE_RUN_Y_OFFSET = 8.0;
const int KNIFE_IDLE_DRAW_W = 220, KNIFE_IDLE_DRAW_H = 286;
const double KNIFE_IDLE_Y_OFFSET = 0.0;

// =========================================================
// ENEMIES & GUARDS SIZING
// =========================================================
const int NINJA_DRAW_W = 180;
const int NINJA_DRAW_H = 240;
const double NINJA_Y_OFFSET = -20.0;

const int GUARD_DRAW_W = 160;
const int GUARD_DRAW_H = 355;
const double GUARD_Y_OFFSET = -75.0;
const double GUARD_DEAD_Y_OFFSET = -180.0;

// Level 4 new-enemy draw sizes - tune these once the art is in,
// the same way L4_GROUND_Y got tuned to the floor line.
const int BOSS_DRAW_W = 260, BOSS_DRAW_H = 340;
const double BOSS_Y_OFFSET = 0.0;

const int MAID_DRAW_W = 150, MAID_DRAW_H = 280;
const double MAID_Y_OFFSET = 0.0;

const int HALLWAY_FOE_DRAW_W = 160, HALLWAY_FOE_DRAW_H = 240;
const double HALLWAY_FOE_Y_OFFSET = 0.0;

const int LIBRARY_SPIDER_DRAW_W = 170, LIBRARY_SPIDER_DRAW_H = 110;
const double LIBRARY_SPIDER_Y_OFFSET = -10.0;

// ---------------------------------------------------------
// 3. PLAYER & WEAPON FRAMES
// ---------------------------------------------------------
unsigned int idleFrame = 0;

const int WALK_FRAME_COUNT = 12;
unsigned int walkRightFrames[WALK_FRAME_COUNT];

const int WALK_GUN_FRAME_COUNT = 4;
unsigned int walkGunFrames[WALK_GUN_FRAME_COUNT];

const int WALK_RPG_FRAME_COUNT = 5;
unsigned int walkRpgFrames[WALK_RPG_FRAME_COUNT];

const int RPG_WALK_DRAW_W[WALK_RPG_FRAME_COUNT] = { 160, 160, 160, 160, 160 };
const int RPG_WALK_DRAW_H[WALK_RPG_FRAME_COUNT] = { 380, 380, 380, 380, 380 };
const int RPG_WALK_PAD_BOTTOM[WALK_RPG_FRAME_COUNT] = { 14,  14,  14,  8,  8 };
const int RPG_WALK_HIP_OFFSET_X[WALK_RPG_FRAME_COUNT] = { 129, 129, 129, 129, 129 };

const int KATANA_ATTACK_FRAME_COUNT = 6;
unsigned int katanaAttackFrames[KATANA_ATTACK_FRAME_COUNT];

const int KATANA_RUN_FRAME_COUNT = 3;
unsigned int katanaRunFrames[KATANA_RUN_FRAME_COUNT];

const double WALK_FRAME_DURATION = 0.08;

const int JUMP_FRAME_COUNT = 9;
unsigned int jumpFrames[JUMP_FRAME_COUNT];
const int JUMP_GUN_FRAME_COUNT = 5;
unsigned int jumpGunFrames[JUMP_GUN_FRAME_COUNT];
const double JUMP_FRAME_DURATION = 0.19;
const int JUMP_GUN_DRAW_H = 260;
const int JUMP_GUN_DRAW_W[JUMP_GUN_FRAME_COUNT] = { 130, 130, 130, 130, 130 };

// ---------------------------------------------------------
// 4. GUN-POINTING POSES
// ---------------------------------------------------------
const int GUN_FRAME_COUNT = 7;
unsigned int gunFrames[GUN_FRAME_COUNT];
const int GUN_DOWN3 = 0, GUN_DOWN2 = 1, GUN_DOWN1 = 2, GUN_NEUTRAL = 3, GUN_UP1 = 4, GUN_UP2 = 5, GUN_UP3 = 6;
const int GUN_SPRITE_DRAW_W = 140, GUN_SPRITE_DRAW_H = 260;
const double GUN_SPRITE_Y_OFFSET = 8.0, AIM_DEADZONE = 18.0, AIM_TIER1 = 70.0, AIM_TIER2 = 160.0;
const double GUN_SRC_W = 446.0, GUN_SRC_H = 560.0;

struct NozzleOffset { double dx; double dy; };
NozzleOffset gunNozzle[GUN_FRAME_COUNT] = {
    { 445.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 222.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 441.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 203.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 445.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 173.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 431.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 131.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 417.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 150.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 397.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 101.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
    { 390.0 / GUN_SRC_W * GUN_SPRITE_DRAW_W, (GUN_SRC_H - 65.0) / GUN_SRC_H * GUN_SPRITE_DRAW_H },
};

// ---------------------------------------------------------
// 5. ENEMY TEXTURES & ANIMATIONS
// ---------------------------------------------------------
unsigned int enemy1IdleTex = 0, enemy1HitTex = 0, enemy1FallTex = 0;
unsigned int enemy2IdleTex = 0, enemy2FallTex = 0, enemy2DeadTex = 0;

unsigned int ninjaDeadTex = 0;
const double NINJA_DEAD_Y_OFFSET = -100.0;

const int NINJA_RUN_FRAME_COUNT = 4;
unsigned int ninjaRunFrames[NINJA_RUN_FRAME_COUNT];
const int NINJA_ATTACK_FRAME_COUNT = 5;
unsigned int ninjaAttackFrames[NINJA_ATTACK_FRAME_COUNT];

unsigned int redNinjaRunFrames[NINJA_RUN_FRAME_COUNT];
unsigned int redNinjaAttackFrames[NINJA_ATTACK_FRAME_COUNT];

// g-01/g-02/g-03 (patrol walk) + g-dead (corpse) - used by Level
// 3's stealth room (roomGuard, Enemy.h/Level3.h) AND now also by
// Level 4's Armory guard (armoryGuard, Enemy.h/Level4.h).
const int GUARD_FRAME_COUNT = 3;
unsigned int guardFrames[GUARD_FRAME_COUNT];
unsigned int guardDeadTex = 0;

// ---------------------------------------------------------
// 6. LOAD EVERYTHING
// ---------------------------------------------------------
void loadAllAssets()
{
    loadingBgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//loading_bg.jpeg");
    menuBgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//menu_bg.jpeg");

    level1BgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//01.jpeg");
    level1Scene2BgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//02.jpeg");
    level1Scene3BgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//03.jpeg");

    for (int i = 0; i < BUILDING_TILE_COUNT; i++)
    {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//building_level2_%d.jpg", i + 1);
        buildingFloorTextures[i] = iLoadImage(path);
    }
    smokeCloudTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Effects//smoke_cloud_v2.png");
    keyTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Items//key.png");
    mannequinTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Items//mannequin.png");

    level3IntroTexture[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_0.jpeg");
    level3IntroTexture[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3-2.jpeg");
    level3IntroTexture[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_1.jpeg");

    level3StartBgTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_3.jpeg");
    level3BlastTexture[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_4.jpeg");
    level3BlastTexture[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_5.jpeg");
    level3AftermathTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_6.jpeg");
    level3InteriorTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_7.jpeg");
    level3Interior2Texture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_8.jpeg");
    level3DoorChoiceTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_8_1.jpeg");
    level3TigerAtriumTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//TigerAtrium.jpeg");
    l3TigerRunFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerRunning1.png");
    l3TigerRunFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerRunning2.png");
    l3TigerRunFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerRunning3.png");
    l3TigerRunFrames[3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerRunning4.png");
    l3TigerRunFrames[4] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerRunning5.png");
    l3TigerAttackTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//TigerAttacking.png");

    level3VaultTex[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_9.jpeg");
    level3VaultTex[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_10.jpeg");
    level3VaultTex[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_11.jpeg");
    level3VaultTex[3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//3_12.jpeg");

    // Level 4 - Imperial Palace room backgrounds
    level4RoomTex[L4_GRAND_ATRIUM_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//grandAtrium_1.jpg");
    level4RoomTex[L4_GRAND_ATRIUM_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//grandAtrium_2.jpg");
    level4RoomTex[L4_GRAND_ATRIUM_3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//grandAtrium_3.jpg");

    level4RoomTex[L4_COLONADE_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//colonade_1.jpg");
    level4RoomTex[L4_COLONADE_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//colonade_2.jpg");
    level4RoomTex[L4_COLONADE_3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//colonade_3.jpg");

    level4RoomTex[L4_TREASURY_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//treasury_1.jpg");
    level4RoomTex[L4_TREASURY_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//treasury_2.jpg");
    level4RoomTex[L4_TREASURY_3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//treasury_3.jpg");

    level4RoomTex[L4_SECRET_ARCHIVES_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//secretArchieves_1.jpg");
    level4RoomTex[L4_SECRET_ARCHIVES_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//secretArchieves_2.jpg");
    level4RoomTex[L4_SECRET_ARCHIVES_3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//secretArchieves_3.jpg");

    level4RoomTex[L4_ARMORY_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//armory_1.jpg");
    level4RoomTex[L4_ARMORY_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//armory_2.jpg");

    level4RoomTex[L4_IMPERIAL_THRONE_1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//imperialthrone_1.jpg");
    level4RoomTex[L4_IMPERIAL_THRONE_2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//imperialthrone_2.jpg");
    level4RoomTex[L4_IMPERIAL_THRONE_3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//imperialthrone_3.jpg");
    level4StairIntroTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Backgrounds//4_0.jpeg");

    // Execution Dais backgrounds removed - the boss fight now
    // happens in the Imperial Throne room instead (see Level4.h).

    // Level 4 enemies (entrance/armory reuse the ninja/guard
    // textures below - no separate load needed for those)
    bossWalkFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_1.png");
    bossWalkFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_2.png");
    bossReserveTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_3.png");
    bossAttackTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_4.png");
    bossRecoverTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_5.png");
    bossDeadTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//bossenemy_7.png");
    // bossenemy_6.png is not currently loaded/used - free to wire in later.

    maidWalkFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//maidenemy_1.png");
    maidWalkFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//maidenemy_2.png");
    maidWalkFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//maidenemy_3.png");
    maidWalkFrames[3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//maidenemy_4.png");

    hallwayWalkFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_1.png");
    hallwayWalkFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_2.png");
    hallwayWalkFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_3.png");
    hallwayWalkFrames[3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_4.png");
    hallwayWalkFrames[4] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_5.png");
    hallwayWalkFrames[5] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_6.png");
    hallwayWalkFrames[6] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_7.png");
    hallwayDeadTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//hallway_8.png");

    spiderWalkFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//libraryspider_1.png");
    spiderWalkFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//libraryspider_2.png");
    spiderWalkFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//libraryspider_3.png");
    spiderWalkFrames[3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//libraryspider_4.png");

    rpgPropTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//RPG//RPG.png");
    rpgLiftFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//RPG//LIftingRPG_01.png");
    rpgLiftFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//RPG//LIftingRPG_02.png");
    rpgLiftFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//RPG//LIftingRPG_03.png");
    rpgStandTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//RPG//StandingStill.png");

    idleFrame = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Standing//1.png");

    for (int i = 0; i < WALK_FRAME_COUNT; i++) {
        char path[100];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//WalkRight//tile%03d.png", i + 1);
        walkRightFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < WALK_GUN_FRAME_COUNT; i++) {
        char path[100];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//WalkingWithGun//%d.png", i + 1);
        walkGunFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < WALK_RPG_FRAME_COUNT; i++) {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//RPG//Walking_RPG//%d.png", i + 1);
        walkRpgFrames[i] = iLoadImage(path);
    }

    for (int i = 0; i < KATANA_ATTACK_FRAME_COUNT; i++) {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Katana//0%d.png", i + 1);
        katanaAttackFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < KATANA_RUN_FRAME_COUNT; i++) {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Katana//Running//0%d.png", i + 1);
        katanaRunFrames[i] = iLoadImage(path);
    }

    knifeIdleTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Knife//idle.png");
    ceilingHangTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Katana//Ceiling.png");
    ceilingAttackTexture = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Katana//CeilingAttack.png");
    l3SpiderFrames[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3spider1.png");
    l3SpiderFrames[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3spider2.png");
    l3SpiderFrames[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3spider3.png");
    tornPlayerTextures[0] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Katana//TearedUP1.png");
    tornPlayerTextures[1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Katana//TearedUP2.png");
    tornPlayerTextures[2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Katana//TearedUP3.png");
    for (int i = 0; i < KNIFE_WALK_FRAME_COUNT; i++) {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Knife//walk_right_%d.png", i + 1);
        knifeWalkRightFrames[i] = iLoadImage(path);
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Knife//walk_left_%d.png", i + 1);
        knifeWalkLeftFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < KNIFE_ATTACK_FRAME_COUNT; i++) {
        char path[150];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Knife//attack_right_%d.png", i + 1);
        knifeAttackRightFrames[i] = iLoadImage(path);
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Knife//attack_left_%d.png", i + 1);
        knifeAttackLeftFrames[i] = iLoadImage(path);
    }

    for (int i = 0; i < JUMP_FRAME_COUNT; i++) {
        char path[100];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Jumping//tile%03d-removebg-preview.png", i + 1);
        jumpFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < JUMP_GUN_FRAME_COUNT; i++) {
        char path[100];
        sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//JumpingWithGun//%d.png", i + 1);
        jumpGunFrames[i] = iLoadImage(path);
    }

    gunFrames[GUN_DOWN3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointDown3.png");
    gunFrames[GUN_DOWN2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointDown2.png");
    gunFrames[GUN_DOWN1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointDown1.png");
    gunFrames[GUN_NEUTRAL] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//Neutral.png");
    gunFrames[GUN_UP1] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointUp1.png");
    gunFrames[GUN_UP2] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointUp2.png");
    gunFrames[GUN_UP3] = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//PointingGun//pointUp3.png");

    enemy1IdleTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//1stEnemyFiring.png");
    enemy1HitTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//1stEnemyTakingDamage.png");
    enemy1FallTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//1stEnemyFalling.png");
    enemy2IdleTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//2ndEnemyFiring.png");
    enemy2FallTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//2ndEnemyFalling.png");
    enemy2DeadTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//2ndEnemyDied.png");

    for (int i = 0; i < NINJA_RUN_FRAME_COUNT; i++) {
        char path[150]; sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3_BlueEnemy_0%d.png", i);
        ninjaRunFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < NINJA_ATTACK_FRAME_COUNT; i++) {
        char path[150]; sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3_BlueEnemy_0%d.png", i + 4);
        ninjaAttackFrames[i] = iLoadImage(path);
    }
    ninjaDeadTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3_BlueEnemy_dead.png");

    for (int i = 0; i < NINJA_RUN_FRAME_COUNT; i++) {
        char path[150]; sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3_RedEnemy_0%d.png", i + 1);
        redNinjaRunFrames[i] = iLoadImage(path);
    }
    for (int i = 0; i < NINJA_ATTACK_FRAME_COUNT; i++) {
        char path[150]; sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Enemy//L3_RedEnemy_0%d.png", i + 5);
        redNinjaAttackFrames[i] = iLoadImage(path);
    }

    for (int i = 0; i < GUARD_FRAME_COUNT; i++) {
        char path[150]; sprintf(path, "iGraphics files (with Updated Keyboard Functions)//Images//Enemy//g-0%d.png", i + 1);
        guardFrames[i] = iLoadImage(path);
    }
    guardDeadTex = iLoadImage("iGraphics files (with Updated Keyboard Functions)//Images//Enemy//g-dead.png");
}
#endif