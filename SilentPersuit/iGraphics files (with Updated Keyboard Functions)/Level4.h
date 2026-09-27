#ifndef LEVEL4_H
#define LEVEL4_H

// =========================================================
// SILENT PURSUIT - Level4.h
// ---------------------------------------------------------
// Level 4 walks the player through a fixed SEQUENCE of palace
// backgrounds - grand atrium, colonnade, treasury, secret
// archives, armory, imperial throne room (see
// level4RoomSequence below, and Level4RoomId / level4RoomTex in
// Assets.h). The Execution Dais area has been removed entirely;
// the final boss fight now happens in the Imperial Throne room.
//
// A room advances two ways:
//   1. Walking to the right edge of the screen, or
//   2. Pressing SPACE at any time, so you can jump ahead
//      whenever you want without needing to walk there.
//   (Both are BLOCKED while the Imperial Throne boss is still
//   alive - see the bossBlockingAdvance check in updateLevel4().)
//
// Each new background gets a short letterbox + fade-in
// "screening" moment, matching the look of Level 3's intro
// cinematic - black bars top/bottom, plus a quick fade up from
// black when the shot first appears (drawn in Screens.h).
//
// ENEMIES: one FOE TYPE per AREA (all shots of that area share
// it), defined in Enemy.h section 9:
//   grand atrium     -> katanaFoe    (melee, walks toward you)
//   colonnade        -> katanaFoe    (melee, walks toward you)
//   treasury         -> hallwayFoe   (melee, walks toward you)
//   secret archives  -> spider       (melee, walks toward you)
//   armory           -> armoryGuard  (patrols full width, backstab-killable)
//   imperial throne  -> boss (room 1 only; room 2 is the victory screen)
// All active enemy types take damage from katana swings, checked
// below in damageCurrentRoomEnemy() the same way Level 3 checks
// its ninjas.
//
// maid (from Enemy.h) is not currently assigned to any room -
// it's still there, just unused until you want it somewhere.
//
// HOW MANY ENEMIES PER WAVE ROOM (enemyCountForRoom() below):
//   Each of the 12 wave-room shots (grand atrium x3, colonade x3,
//   treasury x3, secret archives x3) has its OWN fixed count,
//   following this exact sequence, in room order:
//       2, 4, 3, 2, 3, 4, 2, 4, 3, 2, 3, 4
//   Armory and Imperial Throne 1 each send exactly 1 enemy (the
//   patrolling guard, and the boss, respectively) - see
//   spawnEnemyForRoom(). Imperial Throne 2 sends none; it's the
//   victory screen (see Screens.h).
//
// FRONT / BACK SPAWN ALTERNATION (wave rooms only):
//   Within a wave room, enemies alternate which direction they
//   appear from: the 1st, 3rd, 5th... enemy appears AHEAD of the
//   player, and the 2nd, 4th, 6th... appears BEHIND the player.
//   The Armory guard and the boss don't use this system - the
//   guard patrols on its own fixed loop, and the boss spawns
//   immediately when you enter the throne room.
//
// ARMORY HIDING ZONE:
//   Unlike every other Level 4 room (where holding S hides you
//   anywhere), the Armory only lets you hide within a specific
//   zone (see ARMORY_HIDE_ZONE_MIN_X/MAX_X) where the crates are
//   drawn (Screens.h) - matching the crate art, and how Level 3's
//   rubble-hiding room works.
//
// STAIR-CLIMB ANIMATION:
//   While l4StairIntroActive is true (the opening walk up the
//   palace stairs, see level4StairY()), the player's run/walk
//   animation is driven by the SAME updatePlayerWalkAnimation()
//   call the rest of Level 4 uses (see updateLevel4() below) -
//   it no longer hand-rolls its own frame-stepping copy. That
//   keeps the stair-climb animation identical to normal ground
//   movement (same frame set, same WALK_FRAME_DURATION pacing)
//   instead of risking it drifting out of sync if the shared
//   animation is ever tuned.
// =========================================================

#include "Config.h"
#include "Assets.h"
#include "Player.h"
#include "Enemy.h"

const double L4_WALK_SPEED = 3.0;
const int L4_KNIFE_SWING_DAMAGE = 75;       // more powerful than the old katana's 50 per swing
const double L4_KATANA_HIT_RANGE = 120.0;   // how close the enemy has to be for a swing to land

// Set this to true only once every file in Images/Knife/ (21 total,
// see Player.h/Assets.h) actually exists and you've confirmed it in
// game. Until then this stays false and Level 4 safely uses the
// katana art, which is proven to work - relying on iLoadImage
// silently failing was NOT a safe way to detect a missing file.
const bool L4_USE_KNIFE = false;

// How high up the screen the palace floor actually sits, in
// iGraphics screen pixels (0 = bottom of the window). If a
// character floats above the floor, LOWER this number; if they
// sink below it, RAISE it. Nudge by ~10-15 at a time and rebuild.
const double L4_GROUND_Y = 30.0;
const double L4_STAIR_START_X = 120.0;
const double L4_STAIR_END_X = 690.0;
const double L4_STAIR_START_Y = 32.0;
const double L4_STAIR_END_Y = 390.0;
bool l4StairIntroActive = false;

// --- TEMPORARY DEBUG ACCUMULATORS - remove once the stair-climb
// animation bug is found. A single screenshot always lands on one
// exact tick, which can easily coincide with a moment dx happens to
// be 0 even during a real, continuous hold - so instead of trusting
// one instantaneous sample, these accumulate across every tick since
// the stairs started, so ANY screenshot shows the true history.
int dbgTotalTicks = 0;         // every tick since the stairs began
int dbgMovingTicks = 0;        // ticks where fabs(dx) > 0.5 was true
int dbgFramesSeenMask = 0;     // bit i set => walkFrame reached i at least once while moving
double dbgMaxAbsDx = 0.0;      // largest |dx| ever observed on the stairs

double level4StairY(double x)
{
    double progress = (x - L4_STAIR_START_X) / (L4_STAIR_END_X - L4_STAIR_START_X);
    progress = clampD(progress, 0.0, 1.0);
    return L4_STAIR_START_Y + progress * (L4_STAIR_END_Y - L4_STAIR_START_Y);
}

// How long the fade-up-from-black lasts each time a new
// background first appears (Level 3's intro screening effect).
const double L4_FADE_IN_TIME = 0.6;

// ---------------------------------------------------------
// WHICH FOE TYPE A ROOM USES (so the spawn/wave code can stay
// generic instead of one big switch repeated several times)
// ---------------------------------------------------------
enum L4FoeKind { L4_FOE_NONE, L4_FOE_KATANA, L4_FOE_HALLWAY, L4_FOE_SPIDER, L4_FOE_GUARD, L4_FOE_BOSS };

L4FoeKind foeKindForRoom(Level4RoomId room)
{
    switch (room)
    {
    case L4_GRAND_ATRIUM_1: case L4_GRAND_ATRIUM_2: case L4_GRAND_ATRIUM_3:
    case L4_COLONADE_1:     case L4_COLONADE_2:     case L4_COLONADE_3:
        return L4_FOE_KATANA;
    case L4_TREASURY_1: case L4_TREASURY_2: case L4_TREASURY_3:
        return L4_FOE_HALLWAY;
    case L4_SECRET_ARCHIVES_1: case L4_SECRET_ARCHIVES_2: case L4_SECRET_ARCHIVES_3:
        return L4_FOE_SPIDER;
    case L4_ARMORY_1: case L4_ARMORY_2:
        return L4_FOE_GUARD;
    case L4_IMPERIAL_THRONE_1:
        return L4_FOE_BOSS;
    default:
        return L4_FOE_NONE;   // imperial throne 2 (victory screen) / 3 - no enemy
    }
}

bool isArmoryRoom(Level4RoomId room)
{
    return room == L4_ARMORY_1 || room == L4_ARMORY_2;
}

// ---------------------------------------------------------
// HOW MANY ENEMIES (TOTAL, ONE AFTER ANOTHER) EACH SPECIFIC
// ROOM SENDS
// ---------------------------------------------------------
// Each of the 12 wave-room shots gets its own exact number from
// this fixed sequence, in room order:
//     Grand Atrium 1/2/3   -> 2, 4, 3
//     Colonade     1/2/3   -> 2, 3, 4
//     Treasury     1/2/3   -> 2, 4, 3
//     Secret Arch. 1/2/3   -> 2, 3, 4
// Armory and Imperial Throne 1 always send exactly 1 (their
// enemy doesn't use the wave/front-back system - see
// spawnEnemyForRoom()). Change any individual case below to
// rebalance that one shot.
int enemyCountForRoom(Level4RoomId room)
{
    switch (room)
    {
    case L4_GRAND_ATRIUM_1: return 2;
    case L4_GRAND_ATRIUM_2: return 4;
    case L4_GRAND_ATRIUM_3: return 3;

    case L4_COLONADE_1: return 2;
    case L4_COLONADE_2: return 3;
    case L4_COLONADE_3: return 4;

    case L4_TREASURY_1: return 2;
    case L4_TREASURY_2: return 4;
    case L4_TREASURY_3: return 3;

    case L4_SECRET_ARCHIVES_1: return 2;
    case L4_SECRET_ARCHIVES_2: return 3;
    case L4_SECRET_ARCHIVES_3: return 4;

    case L4_ARMORY_1: case L4_ARMORY_2: return 1;
    case L4_IMPERIAL_THRONE_1: return 1;

    default:
        return 0;   // imperial throne 2 (victory screen) / 3
    }
}

// ---------------------------------------------------------
// ENEMY "APPEARS FROM A DISTANCE" SPAWNING (wave rooms only)
// ---------------------------------------------------------
// No wave enemy spawns the instant a room loads. The very first
// one waits until the player has walked at least this far into
// the background (see checkL4EnemySpawnTrigger()); every enemy
// after that spawns as soon as the current one dies, after a
// short delay (see updateL4WaveSpawns()). Either way, it appears
// this far ahead of / behind the player - see
// L4_ENEMY_SPAWN_AHEAD_DIST / L4_CHAIN_BEHIND_DIST below - and
// then walks toward them, so it's always seen approaching from a
// distance. (The Armory guard and the boss don't use any of this
// - see spawnEnemyForRoom().)
const double L4_ENEMY_SPAWN_TRIGGER_X = 260.0;  // player must walk at least this far in first
const double L4_ENEMY_SPAWN_AHEAD_DIST = 280.0; // how far ahead of the player a "front" spawn appears
const double L4_CHAIN_BEHIND_DIST = 280.0;      // how far behind the player a "back" spawn appears
const double L4_CHAIN_DELAY = 0.6;              // seconds pause before each next enemy shows up

bool l4EnemyPendingSpawn = false;   // this room still needs its (first) wave enemy spawned

int l4EnemiesSpawnedInRoom = 0;   // how many of this room's wave have spawned so far (incl. current)
int l4EnemyTargetCount = 0;       // total this room should send (from enemyCountForRoom)
bool l4NextSpawnScheduled = false; // a delayed next-spawn is already counting down
double l4ChainTimer = 0.0;         // counts down to the next spawn (0 = none pending)

// True for the 1st, 3rd, 5th... enemy in the room (spawns ahead
// of the player / from the front); false for the 2nd, 4th, 6th...
// (spawns behind the player / from the back). spawnIndex is
// 1-based (the 1st enemy in the room is spawnIndex 1).
bool l4SpawnIsFromFront(int spawnIndex)
{
    return (spawnIndex % 2) == 1;
}

// The player always walks into a room from the left (x=80), so
// spawning near the far right means the enemy is visible
// approaching from a distance instead of right next to you.
double randomL4SpawnX()
{
    return (WIN_W - 160.0) + (double)(rand() % 60);   // somewhere between WIN_W-160 and WIN_W-100
}

// ---------------------------------------------------------
// ARMORY HIDING ZONE
// ---------------------------------------------------------
// The x-range (in screen coordinates, since Level 4 doesn't
// scroll) where the crates are drawn (see drawArmoryHidingCrates
// in Screens.h). Holding S only hides the player while they are
// standing inside this zone - unlike every other Level 4 room,
// where holding S hides the player anywhere (see updateLevel4()).
const double ARMORY_HIDE_ZONE_MIN_X = 480.0;
const double ARMORY_HIDE_ZONE_MAX_X = 620.0;

// ---------------------------------------------------------
// ROOM SEQUENCE - this is just the Level4RoomId enum in order.
// Edit Assets.h to add/remove shots, and update
// LEVEL4_SEQUENCE_LENGTH to match. The Execution Dais area has
// been removed, and Imperial Throne 3 is left unused (the fight
// happens on shot 1, the victory screen is shot 2).
// ---------------------------------------------------------
const int LEVEL4_SEQUENCE_LENGTH = 16;
Level4RoomId level4RoomSequence[LEVEL4_SEQUENCE_LENGTH] =
{
    L4_GRAND_ATRIUM_1, L4_GRAND_ATRIUM_2, L4_GRAND_ATRIUM_3,
    L4_COLONADE_1,     L4_COLONADE_2,     L4_COLONADE_3,
    L4_TREASURY_1,     L4_TREASURY_2,     L4_TREASURY_3,
    L4_SECRET_ARCHIVES_1, L4_SECRET_ARCHIVES_2, L4_SECRET_ARCHIVES_3,
    L4_ARMORY_1,       L4_ARMORY_2,
    L4_IMPERIAL_THRONE_1, L4_IMPERIAL_THRONE_2
};

int level4CurrentStep = 0;      // index into level4RoomSequence
double level4StepTimer = 0.0;   // seconds spent on the current step (drives the fade-in)
bool l4SpaceWasHeld = false;    // debounce so holding SPACE doesn't skip multiple rooms per press
bool l4CompletionAudioPlayed = false;

Level4RoomId currentLevel4Room() { return level4RoomSequence[level4CurrentStep]; }

// Explicit range checks since areas don't have a uniform shot
// count (armory only has 2). Order matches the enum order in
// Assets.h. Imperial Throne 3 still resolves correctly here even
// though it's unused in the sequence.
int getLevel4AreaIndex(Level4RoomId room)
{
    if (room <= L4_GRAND_ATRIUM_3)    return 0;
    if (room <= L4_COLONADE_3)        return 1;
    if (room <= L4_TREASURY_3)        return 2;
    if (room <= L4_SECRET_ARCHIVES_3) return 3;
    if (room <= L4_ARMORY_2)          return 4;
    return 5;   // imperial throne (1/2/3)
}


// ---------------------------------------------------------
// SPAWN WHATEVER ENEMY BELONGS TO A ROOM
// ---------------------------------------------------------
// Deactivates every Level 4 enemy first so only the current
// room's enemy is ever live at once.
//   - katana/hallway/spider (wave foes): just ARMS
//     l4EnemyPendingSpawn - the actual spawn happens later in
//     checkL4EnemySpawnTrigger() once the player has walked in
//     far enough.
//   - Armory guard: spawns immediately and starts patrolling the
//     full width right away - no waiting, no wave.
//   - Boss (Imperial Throne 1 only): spawns immediately too,
//     since the throne room is meant to confront you the moment
//     you arrive.
void spawnEnemyForRoom(Level4RoomId room)
{
    katanaFoe.active = false;
    hallwayFoe.active = false;
    maid.active = false;
    spider.active = false;
    boss.active = false;
    armoryGuard.active = false;

    l4EnemyPendingSpawn = false;
    l4EnemiesSpawnedInRoom = 0;
    l4EnemyTargetCount = enemyCountForRoom(room);
    l4NextSpawnScheduled = false;
    l4ChainTimer = 0.0;

    switch (foeKindForRoom(room))
    {
    case L4_FOE_KATANA:
    case L4_FOE_HALLWAY:
    case L4_FOE_SPIDER:
        if (l4EnemyTargetCount > 0) l4EnemyPendingSpawn = true;   // wait for the player to walk in, then spawn ahead of them
        break;
    case L4_FOE_GUARD:
        resetArmoryGuard(WIN_W / 2.0, L4_GROUND_Y);
        l4EnemiesSpawnedInRoom = 1;
        break;
    case L4_FOE_BOSS:
        resetBoss(WIN_W - 160.0, L4_GROUND_Y);
        l4EnemiesSpawnedInRoom = 1;
        break;
    default:
        break;   // imperial throne 2 (victory screen) / 3 - no enemy
    }
}


// ---------------------------------------------------------
// SPAWN THE ROOM'S FIRST WAVE ENEMY ONCE THE PLAYER HAS WALKED
// FAR ENOUGH IN
// ---------------------------------------------------------
// Called every tick while a spawn is pending for the current
// room. Waits for assassin.x to pass L4_ENEMY_SPAWN_TRIGGER_X,
// then places the room's enemy L4_ENEMY_SPAWN_AHEAD_DIST pixels
// ahead of the player (clamped on-screen) - this is always the
// "1st" enemy in the room, so it's always a front spawn (see
// l4SpawnIsFromFront()). Only applies to katana/hallway/spider -
// the Armory guard and boss are spawned directly in
// spawnEnemyForRoom() instead.
void checkL4EnemySpawnTrigger()
{
    if (!l4EnemyPendingSpawn) return;
    if (assassin.x < L4_ENEMY_SPAWN_TRIGGER_X) return;

    double spawnX = clampD(assassin.x + L4_ENEMY_SPAWN_AHEAD_DIST, 20.0, WIN_W - 20.0);

    switch (foeKindForRoom(currentLevel4Room()))
    {
    case L4_FOE_KATANA:  resetKatanaFoe(spawnX, L4_GROUND_Y);  break;
    case L4_FOE_HALLWAY: resetHallwayFoe(spawnX, L4_GROUND_Y); break;
    case L4_FOE_SPIDER:  resetSpider(spawnX, L4_GROUND_Y);     break;
    default: break;
    }

    l4EnemiesSpawnedInRoom = 1;
    l4EnemyPendingSpawn = false;
}


// ---------------------------------------------------------
// IS THIS ROOM'S CURRENT (WAVE) ENEMY DEAD RIGHT NOW?
// ---------------------------------------------------------
bool isCurrentRoomFoeDead(L4FoeKind kind)
{
    switch (kind)
    {
    case L4_FOE_KATANA:  return katanaFoe.active && !katanaFoe.alive;
    case L4_FOE_HALLWAY: return hallwayFoe.active && !hallwayFoe.alive;
    case L4_FOE_SPIDER:  return spider.active && !spider.alive;
    default: return false;
    }
}


// ---------------------------------------------------------
// KEEP THE WAVE GOING - SPAWN THE NEXT ENEMY (SAME TYPE) ONCE
// THE CURRENT ONE DIES, ALTERNATING FRONT/BACK, UNTIL THE ROOM'S
// TARGET COUNT IS REACHED
// ---------------------------------------------------------
// Called every tick for any room that has a katana/hallway/spider
// foe. As soon as the current one dies, if more are still owed
// for this room (l4EnemiesSpawnedInRoom < l4EnemyTargetCount), it
// schedules the next spawn after a short delay. That next enemy
// spawns from the front (ahead of the player) if it's the 1st,
// 3rd, 5th... of the room, or from the back (behind the player)
// if it's the 2nd, 4th, 6th... - see l4SpawnIsFromFront(). Never
// runs for the Armory guard or the boss.
void updateL4WaveSpawns()
{
    L4FoeKind kind = foeKindForRoom(currentLevel4Room());
    if (kind == L4_FOE_NONE || kind == L4_FOE_BOSS || kind == L4_FOE_GUARD) return;

    bool moreToSend = (l4EnemiesSpawnedInRoom < l4EnemyTargetCount);

    if (isCurrentRoomFoeDead(kind) && moreToSend && !l4NextSpawnScheduled)
    {
        l4NextSpawnScheduled = true;
        l4ChainTimer = L4_CHAIN_DELAY;
    }

    if (l4ChainTimer > 0.0)
    {
        l4ChainTimer -= TICK;
        if (l4ChainTimer <= 0.0)
        {
            l4ChainTimer = 0.0;
            l4NextSpawnScheduled = false;

            int nextIndex = l4EnemiesSpawnedInRoom + 1;
            bool fromFront = l4SpawnIsFromFront(nextIndex);

            double spawnX = fromFront
                ? clampD(assassin.x + L4_ENEMY_SPAWN_AHEAD_DIST, 20.0, WIN_W - 20.0)
                : clampD(assassin.x - L4_CHAIN_BEHIND_DIST, 20.0, WIN_W - 20.0);

            switch (kind)
            {
            case L4_FOE_KATANA:  resetKatanaFoe(spawnX, L4_GROUND_Y);  break;
            case L4_FOE_HALLWAY: resetHallwayFoe(spawnX, L4_GROUND_Y); break;
            case L4_FOE_SPIDER:  resetSpider(spawnX, L4_GROUND_Y);     break;
            default: break;
            }
            l4EnemiesSpawnedInRoom = nextIndex;
        }
    }
}


// ---------------------------------------------------------
// START / RESTART LEVEL 4
// ---------------------------------------------------------
void resetLevel4()
{
    level4CurrentStep = 0;
    level4StepTimer = 0.0;
    l4SpaceWasHeld = false;
    l4CompletionAudioPlayed = false;

    assassin.x = 80.0;
    assassin.y = L4_GROUND_Y;
    assassin.dx = 0.0; assassin.dy = 0.0;
    assassin.isJumping = false;
    assassin.facingRight = true;
    assassin.walkFrame = 0; assassin.walkFrameTimer = 0.0;

    gunEquipped = false;
    isRpgEquipped = false;
    isKatanaEquipped = !L4_USE_KNIFE;
    isKnifeEquipped = L4_USE_KNIFE;
    isKatanaAttacking = false;
    isHiding = false;

    playerHP = PLAYER_MAX_HP;

    l4StairIntroActive = false;
    assassin.x = 80.0;
    assassin.y = L4_GROUND_Y;

    dbgTotalTicks = 0;
    dbgMovingTicks = 0;
    dbgFramesSeenMask = 0;
    dbgMaxAbsDx = 0.0;

    spawnEnemyForRoom(currentLevel4Room());
}


// ---------------------------------------------------------
// MOVE TO THE NEXT STEP IN THE SEQUENCE
// ---------------------------------------------------------
void advanceLevel4Room()
{
    Level4RoomId previousRoom = currentLevel4Room();
    level4CurrentStep++;

    if (level4CurrentStep >= LEVEL4_SEQUENCE_LENGTH)
    {
        // Walked/skipped past the end of the sequence. Shouldn't
        // normally happen now that Imperial Throne 2 is the last
        // step and is a dead end (handled by the Game Completed
        // button in Screens.h/Main.cpp) - kept as a safety net.
        state = MENU;
        return;
    }

    Level4RoomId newRoom = currentLevel4Room();

    if (newRoom == L4_IMPERIAL_THRONE_2 && !l4CompletionAudioPlayed)
    {
        playGameAudio("LEVEL_COMPLETED.mp3");
        l4CompletionAudioPlayed = true;
    }

    // Only play the fade-in cinematic when actually entering a new
    // AREA (e.g. grandAtrium -> colonade). Moving between shots of
    // the same area (grandAtrium_1 -> grandAtrium_2) doesn't fade.
    if (getLevel4AreaIndex(newRoom) != getLevel4AreaIndex(previousRoom))
        level4StepTimer = 0.0;

    assassin.x = 80.0;          // walk in from the left edge of the new screen
    assassin.y = L4_GROUND_Y;

    spawnEnemyForRoom(newRoom);
}


// ---------------------------------------------------------
// APPLY A KATANA SWING TO WHICHEVER ENEMY IS ACTIVE
// ---------------------------------------------------------
void damageCurrentRoomEnemy()
{
    switch (foeKindForRoom(currentLevel4Room()))
    {
    case L4_FOE_KATANA:
        if (katanaFoe.active && katanaFoe.alive && fabs(katanaFoe.x - assassin.x) < L4_KATANA_HIT_RANGE)
            damageKatanaFoe(L4_KNIFE_SWING_DAMAGE);
        break;
    case L4_FOE_HALLWAY:
        if (hallwayFoe.active && hallwayFoe.alive && fabs(hallwayFoe.x - assassin.x) < L4_KATANA_HIT_RANGE)
            damageHallwayFoe(L4_KNIFE_SWING_DAMAGE);
        break;
    case L4_FOE_SPIDER:
        if (spider.active && spider.alive && fabs(spider.x - assassin.x) < L4_KATANA_HIT_RANGE)
            damageSpider(L4_KNIFE_SWING_DAMAGE);
        break;
    case L4_FOE_GUARD:
        if (armoryGuard.active && armoryGuard.alive && fabs(armoryGuard.x - assassin.x) < L4_KATANA_HIT_RANGE)
            damageArmoryGuard(L4_KNIFE_SWING_DAMAGE);
        break;
    case L4_FOE_BOSS:
        if (boss.active && boss.bState != BOSS_DEAD && fabs(boss.x - assassin.x) < L4_KATANA_HIT_RANGE)
            damageBoss(L4_KNIFE_SWING_DAMAGE);
        break;
    default:
        break;   // imperial throne 2 (victory screen) / 3 - no enemy
    }
}


// ---------------------------------------------------------
// ONE FULL STEP OF LEVEL 4
// ---------------------------------------------------------
void updateLevel4()
{
    level4StepTimer += TICK;

    bool leftHeld = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
    bool rightHeld = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);
    bool downHeld = isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN);

    if (l4StairIntroActive)
    {
        if (leftHeld && !rightHeld) assassin.dx = -L4_WALK_SPEED;
        else if (rightHeld && !leftHeld) assassin.dx = L4_WALK_SPEED;
        else assassin.dx = 0.0;

        assassin.x += assassin.dx;
        assassin.x = clampD(assassin.x, L4_STAIR_START_X, L4_STAIR_END_X);
        assassin.y = level4StairY(assassin.x);
        assassin.isJumping = false;
        isHiding = false;
        updatePlayerFacing(leftHeld, rightHeld);

        // Drive the run/walk animation the SAME way the rest of
        // Level 4 does (see the non-stair branch below), instead
        // of the old hand-rolled copy of this logic, so climbing
        // the stairs animates identically to normal ground
        // movement - same frames, same pacing - and never drifts
        // out of sync if updatePlayerWalkAnimation() is ever
        // retuned.
        if (fabs(assassin.dx) > 0.5)
        {
            updatePlayerWalkAnimation();
        }
        else
        {
            assassin.walkFrame = 0;
            assassin.walkFrameTimer = 0.0;
        }

        // TEMPORARY DEBUG - see the comment by dbgTotalTicks above.
        dbgTotalTicks++;
        if (fabs(assassin.dx) > dbgMaxAbsDx) dbgMaxAbsDx = fabs(assassin.dx);
        if (fabs(assassin.dx) > 0.5)
        {
            dbgMovingTicks++;
            if (assassin.walkFrame >= 0 && assassin.walkFrame < 31)
                dbgFramesSeenMask |= (1 << assassin.walkFrame);
        }

        if (assassin.x >= L4_STAIR_END_X)
        {
            l4StairIntroActive = false;
            assassin.x = 80.0;
            assassin.y = L4_GROUND_Y;
            assassin.dx = 0.0;
            level4StepTimer = 0.0;
            spawnEnemyForRoom(currentLevel4Room());
        }
        return;
    }

    // Hold S to hide. Everywhere except the Armory this works
    // anywhere in the room (patrolling melee foes won't notice
    // you while hidden); in the Armory it ONLY works inside the
    // crate zone (see ARMORY_HIDE_ZONE_MIN_X/MAX_X and the crates
    // drawn in Screens.h), matching how Level 3's rubble-hiding
    // room works.
    bool armoryRoom = isArmoryRoom(currentLevel4Room());
    bool inArmoryHideZone = (assassin.x > ARMORY_HIDE_ZONE_MIN_X && assassin.x < ARMORY_HIDE_ZONE_MAX_X);
    bool canHideHere = armoryRoom ? inArmoryHideZone : true;

    if (downHeld && canHideHere && !isKatanaAttacking)
    {
        isHiding = true;
        assassin.dx = 0.0;
    }
    else
    {
        isHiding = false;
        if (isKatanaAttacking) assassin.dx = 0.0;
        else if (leftHeld && !rightHeld) assassin.dx = -L4_WALK_SPEED;
        else if (rightHeld && !leftHeld) assassin.dx = L4_WALK_SPEED;
        else assassin.dx = 0.0;
    }

    assassin.x += assassin.dx;
    assassin.x = clampD(assassin.x, 60.0, WIN_W - 60.0);

    if (!isHiding)
    {
        updatePlayerFacing(leftHeld, rightHeld);
        updatePlayerWalkAnimation();
    }

    // Spawn this room's first wave enemy (if pending) once the
    // player has walked in far enough - see checkL4EnemySpawnTrigger().
    checkL4EnemySpawnTrigger();

    // Only the current room's enemy is ever active, so it's safe
    // to just call all these update functions - the inactive ones
    // return immediately.
    updateKatanaFoe();
    updateHallwayFoe();
    updateMaid();
    updateSpider();
    updateArmoryGuard();
    updateBoss();

    // Keep sending enemies (alternating front/back) until this
    // room's target count (enemyCountForRoom) is reached. Skips
    // itself for the Armory guard and the boss.
    updateL4WaveSpawns();

    if (isKatanaAttacking)
    {
        katanaAttackTimer += TICK;
        if (katanaAttackTimer > 0.06)
        {
            katanaAttackTimer = 0.0;
            katanaAttackFrame++;

            if (katanaAttackFrame == 2 && !hasPlayerDealtDamageThisSwing)
            {
                damageCurrentRoomEnemy();
                hasPlayerDealtDamageThisSwing = true;
            }

            if (katanaAttackFrame >= 5)
            {
                isKatanaAttacking = false;
                katanaAttackFrame = 0;
                hasPlayerDealtDamageThisSwing = false;
            }
        }
    }

    // The Imperial Throne boss fight can't be skipped by just
    // walking past or pressing SPACE - both are blocked here
    // while it's still alive.
    bool bossBlockingAdvance = (foeKindForRoom(currentLevel4Room()) == L4_FOE_BOSS
        && boss.active && boss.bState != BOSS_DEAD);

    // Walking off the right edge still advances too.
    if (assassin.x >= WIN_W - 70.0 && !bossBlockingAdvance) advanceLevel4Room();

    if (playerHP <= 0) state = GAME_OVER;
}

#endif