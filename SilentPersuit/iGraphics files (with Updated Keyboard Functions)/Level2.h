#ifndef LEVEL2_H
#define LEVEL2_H

#include "Config.h"
#include "Assets.h"
#include "Draw.h"
#include "World.h"
#include "Player.h"
#include "Enemy.h"
#include <stdlib.h>

const double LEVEL2_CYLINDER_X = 1035.0;
const double LEVEL2_CYLINDER_Y = LEVEL2_FLOOR_1_Y;
const double LEVEL2_MANNEQUIN_X = 2280.0;
const double LEVEL2_MANNEQUIN_Y = LEVEL2_FLOOR_4_Y;
const int CYLINDER_HITS_TO_RELEASE_SMOKE = 5;
const int MANNEQUIN_HITS_TO_BREAK = 2;
const double SMOKE_DURATION = 120.0;
const int SMOKE_CLOUD_COUNT = 24;
const int LEVEL2_TOTAL_ENEMIES = 20;
const double ENEMY_VISION_RANGE = 280.0;
const double ENEMY_MOVE_SPEED = 1.1;
const double LEVEL2_EXIT_X = 3120.0;
const double LEVEL2_EXIT_RADIUS = 65.0;
const double LEVEL2_EXIT_HOLD_TIME = 2.0;
const double LEVEL2_SPAWN_MIN_DISTANCE = 650.0;

int cylinderHitCount = 0;
bool smokeReleased = false;
double smokeTimer = 0.0;
double smokeWorldX[SMOKE_CLOUD_COUNT];
int mannequinHits = 0;
bool mannequinBroken = false;
bool mannequinKeyOnFloor = false;
bool droppedKeyOnFloor[4] = { false, false, false, false };
double droppedKeyX[4] = { 0.0, 0.0, 0.0, 0.0 };
double droppedKeyY[4] = { 0.0, 0.0, 0.0, 0.0 };
int collectedKeyCount = 0;
int enemyKeysDropped = 0;
int enemiesSpawned = 0;
int enemiesKilled = 0;
bool enemySpawningStarted = false;
bool rapidSpawnMode = false;
bool threeEnemySmokeMode = false;
double spawnTimer = 0.0;
bool tenKillDecision = false;
double tenKillTimer = 0.0;
bool waveEnemyAccounted[LEVEL2_WAVE_SIZE] = { false, false, false, false };
bool waveEnemyDropsKey[LEVEL2_WAVE_SIZE] = { false, false, false, false };
int killsSinceReplacement = 0;
double level2ExitHoldTimer = 0.0;
bool level2ExitActivated = false;
bool level2CompletionAudioPlayed = false;

bool level2Complete()
{
    if (enemiesKilled < LEVEL2_TOTAL_ENEMIES) return false;
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
        if (level2WaveEnemies[i].active && level2WaveEnemies[i].alive) return false;
    return true;
}

double floorYFromIndex(int floor)
{
    if (floor == 4) return LEVEL2_FLOOR_4_Y;
    if (floor == 3) return LEVEL2_FLOOR_3_Y;
    if (floor == 2) return LEVEL2_FLOOR_2_Y;
    return LEVEL2_FLOOR_1_Y;
}

void resetLevel2Waves()
{
    enemiesSpawned = enemiesKilled = 0;
    enemySpawningStarted = rapidSpawnMode = tenKillDecision = threeEnemySmokeMode = false;
    spawnTimer = tenKillTimer = 0.0;
    killsSinceReplacement = 0;
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
    {
        level2WaveEnemies[i].active = false;
        level2WaveEnemies[i].alive = false;
        level2WaveEnemies[i].corpseGone = true;
        waveEnemyAccounted[i] = true;
        waveEnemyDropsKey[i] = false;
    }
}

void resetLevel2Smoke()
{
    cylinderHitCount = 0; smokeReleased = false; smokeTimer = 0.0;
    mannequinHits = 0; mannequinBroken = false; mannequinKeyOnFloor = false;
    collectedKeyCount = enemyKeysDropped = 0;
    level2ExitHoldTimer = 0.0;
    level2ExitActivated = false;
    level2CompletionAudioPlayed = false;
    for (int i = 0; i < 4; i++) droppedKeyOnFloor[i] = false;
    for (int i = 0; i < SMOKE_CLOUD_COUNT; i++) smokeWorldX[i] = 80.0 + (rand() % 2900);
}

int livingLevel2Enemies()
{
    int living = 0;
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
        if (level2WaveEnemies[i].active && level2WaveEnemies[i].alive) living++;
    return living;
}

void configureLevel2Enemy(Enemy& e, int floor, double x, bool secondType)
{
    resetEnemy(e, x, floorYFromIndex(floor) - SPRITE_Y_OFFSET, false);
    e.active = true;
    if (secondType) { e.idleTex = enemy2IdleTex; e.hitTex = 0; e.fallTex = enemy2FallTex; e.deadTex = enemy2DeadTex; }
    else { e.idleTex = enemy1IdleTex; e.hitTex = enemy1HitTex; e.fallTex = enemy1FallTex; e.deadTex = 0; }
}

void spawnLevel2Batch(int requested)
{
    int available = LEVEL2_TOTAL_ENEMIES - enemiesSpawned;
    int count = requested < available ? requested : available;
    if (count <= 0) return;

    const double pairX[4] = { 430.0, 610.0, 2580.0, 2760.0 };
    int keyCarrier = (enemyKeysDropped < 4) ? rand() % count : -1;
    int leftPairFloor = 1 + (rand() % 4);
    int rightPairFloor = 1 + (rand() % 4);
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
    {
        waveEnemyAccounted[i] = false; waveEnemyDropsKey[i] = false;
        if (i >= count) { level2WaveEnemies[i].active = false; level2WaveEnemies[i].alive = false; level2WaveEnemies[i].corpseGone = true; waveEnemyAccounted[i] = true; continue; }
        configureLevel2Enemy(level2WaveEnemies[i], i < 2 ? leftPairFloor : rightPairFloor,
                             pairX[i], enemiesSpawned > 0 && (rand() % 2));
        waveEnemyDropsKey[i] = (i == keyCarrier);
    }
    enemiesSpawned += count;
}

void spawnLevel2ReplacementGroup(int requested)
{
    int available = LEVEL2_TOTAL_ENEMIES - enemiesSpawned;
    int count = available < requested ? available : requested;
    if (count <= 0) return;

    // Use a complete indoor pair position that is far from the player. If
    // every position is close, choose the farthest one rather than spawning
    // on top of them.
    const double pairStarts[5] = { 360.0, 780.0, 1640.0, 2340.0, 2740.0 };
    int validChoices[5];
    int validCount = 0;
    int farthestChoice = 0;
    double farthestDistance = -1.0;
    for (int i = 0; i < 5; i++)
    {
        double pairCenter = pairStarts[i] + 75.0;
        double distance = fabs(pairCenter - assassin.x);
        if (distance >= LEVEL2_SPAWN_MIN_DISTANCE) validChoices[validCount++] = i;
        if (distance > farthestDistance) { farthestDistance = distance; farthestChoice = i; }
    }
    int pairChoice = validCount > 0 ? validChoices[rand() % validCount] : farthestChoice;
    double baseX = pairStarts[pairChoice];
    int floor = 1 + (rand() % 4);
    int spawned = 0;
    int keyCarrier = (enemyKeysDropped < 4) ? rand() % count : -1;

    for (int i = 0; i < LEVEL2_WAVE_SIZE && spawned < count; i++)
    {
        if (level2WaveEnemies[i].alive) continue;
        configureLevel2Enemy(level2WaveEnemies[i], floor, baseX + spawned * 150.0, rand() % 2);
        waveEnemyAccounted[i] = false;
        waveEnemyDropsKey[i] = (spawned == keyCarrier);
        spawned++;
    }
    enemiesSpawned += spawned;
}

bool checkLevel2CylinderShot(int aimX, int aimY)
{
    if (currentLevel != 2 || smokeReleased || level2FloorIndex(assassin.y) != 1) return false;
    double sx = worldToScreen(LEVEL2_CYLINDER_X);
    if (aimX < sx - 28.0 || aimX > sx + 28.0 || aimY < LEVEL2_CYLINDER_Y || aimY > LEVEL2_CYLINDER_Y + 95.0) return false;
    cylinderHitCount++;
    if (cylinderHitCount >= CYLINDER_HITS_TO_RELEASE_SMOKE)
    {
        smokeReleased = true; smokeTimer = SMOKE_DURATION;
        if (enemiesKilled >= 10)
        {
            threeEnemySmokeMode = true;
            tenKillDecision = false;

            // Smoke is the successful 10-kill objective: begin the new
            // three-enemy wave immediately, but never add it over survivors.
            if (livingLevel2Enemies() == 0 && enemiesSpawned < LEVEL2_TOTAL_ENEMIES)
            {
                killsSinceReplacement = 0;
                spawnLevel2ReplacementGroup(3);
            }
        }
        for (int i = 0; i < SMOKE_CLOUD_COUNT; i++) smokeWorldX[i] = 80.0 + (rand() % 2900);
    }
    return true;
}

bool checkLevel2MannequinShot(int aimX, int aimY)
{
    if (currentLevel != 2 || mannequinBroken || level2FloorIndex(assassin.y) != 4) return false;
    double sx = worldToScreen(LEVEL2_MANNEQUIN_X);
    if (aimX < sx - 28.0 || aimX > sx + 28.0 || aimY < LEVEL2_MANNEQUIN_Y || aimY > LEVEL2_MANNEQUIN_Y + 100.0) return false;
    mannequinHits++;
    if (mannequinHits >= MANNEQUIN_HITS_TO_BREAK) { mannequinBroken = true; mannequinKeyOnFloor = true; }
    return true;
}

void collectLevel2Keys()
{
    if (mannequinKeyOnFloor && distance2D(assassin.x, assassin.y, LEVEL2_MANNEQUIN_X, LEVEL2_MANNEQUIN_Y) < 65.0)
    {
        mannequinKeyOnFloor = false; collectedKeyCount++;
        if (!enemySpawningStarted) { enemySpawningStarted = true; spawnLevel2Batch(2); }
    }
    for (int i = 0; i < 4; i++)
        if (droppedKeyOnFloor[i] && distance2D(assassin.x, assassin.y, droppedKeyX[i], droppedKeyY[i]) < 60.0)
        { droppedKeyOnFloor[i] = false; collectedKeyCount++; }
}

bool level2ExitUnlocked()
{
    return collectedKeyCount >= 2;
}

void updateLevel2Exit()
{
    if (!level2ExitUnlocked())
    {
        level2ExitHoldTimer = 0.0;
        return;
    }

    bool atExit = level2FloorIndex(assassin.y) == 1 &&
                  fabs(assassin.x - LEVEL2_EXIT_X) <= LEVEL2_EXIT_RADIUS;
    if (!atExit)
    {
        level2ExitHoldTimer = 0.0;
        return;
    }

    level2ExitHoldTimer += TICK;
    if (level2ExitHoldTimer >= LEVEL2_EXIT_HOLD_TIME)
    {
        level2ExitActivated = true;
        if (!level2CompletionAudioPlayed)
        {
            playGameAudio("LEVEL_COMPLETED.mp3");
            level2CompletionAudioPlayed = true;
        }
    }
}

void moveLevel2Enemy(Enemy& e)
{
    int enemyFloor = level2FloorIndex(e.y), playerFloor = level2FloorIndex(assassin.y);

    // Once an enemy has entered a staircase, interpolate both X and Y along
    // that exact diagonal.  It cannot fire, chase, or switch floors until it
    // reaches the other end of the stairs.
    if (e.usingStairs)
    {
        double direction = e.stairEndX >= e.stairStartX ? 1.0 : -1.0;
        e.facingRight = direction > 0.0;
        e.x += direction * ENEMY_MOVE_SPEED;
        if ((direction > 0.0 && e.x >= e.stairEndX) ||
            (direction < 0.0 && e.x <= e.stairEndX))
        {
            e.x = e.stairEndX;
            e.y = floorYFromIndex(e.stairEndFloor) - SPRITE_Y_OFFSET;
            e.usingStairs = false;
            return;
        }

        double length = e.stairEndX - e.stairStartX;
        double progress = (e.x - e.stairStartX) / length;
        double startY = floorYFromIndex(e.stairStartFloor);
        double endY = floorYFromIndex(e.stairEndFloor);
        e.y = startY + (endY - startY) * progress - SPRITE_Y_OFFSET;
        return;
    }

    if (enemyFloor == playerFloor)
    {
        double dx = assassin.x - e.x;
        if (fabs(dx) > 115.0) e.x += (dx > 0.0 ? ENEMY_MOVE_SPEED : -ENEMY_MOVE_SPEED);
        return;
    }

    // Pick the nearest usable staircase entrance.  The first two stairs rise
    // to the right; the far-right staircase rises to the left.
    double stairLeft[3] = { STAIR1_X0, STAIR2_X0, STAIR3_X0 };
    double stairRight[3] = { STAIR1_X1, STAIR2_X1, STAIR3_X1 };
    int nextFloor = enemyFloor + (playerFloor > enemyFloor ? 1 : -1);
    double bestStart = 0.0, bestEnd = 0.0, bestDistance = 1e30;
    for (int i = 0; i < 3; i++)
    {
        bool risesRight = i != 2;
        bool goingUp = nextFloor > enemyFloor;
        double start = (goingUp == risesRight) ? stairLeft[i] : stairRight[i];
        double end = (goingUp == risesRight) ? stairRight[i] : stairLeft[i];
        double distance = fabs(e.x - start);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestStart = start;
            bestEnd = end;
        }
    }

    // They stay on their current floor until their feet reach the first step.
    if (fabs(e.x - bestStart) > ENEMY_MOVE_SPEED)
    {
        e.facingRight = bestStart > e.x;
        e.x += e.facingRight ? ENEMY_MOVE_SPEED : -ENEMY_MOVE_SPEED;
        return;
    }

    e.x = bestStart;
    e.stairStartX = bestStart;
    e.stairEndX = bestEnd;
    e.stairStartFloor = enemyFloor;
    e.stairEndFloor = nextFloor;
    e.usingStairs = true;
}

bool enemySeesPlayer(const Enemy& e)
{
    return !e.usingStairs && level2FloorIndex(e.y) == level2FloorIndex(assassin.y) && fabs(e.x - assassin.x) <= ENEMY_VISION_RANGE;
}

void recordLevel2Deaths()
{
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
    {
        Enemy& e = level2WaveEnemies[i];
        if (!waveEnemyAccounted[i] && !e.alive)
        {
            waveEnemyAccounted[i] = true; enemiesKilled++; killsSinceReplacement++;
            if (waveEnemyDropsKey[i] && enemyKeysDropped < 4)
            {
                droppedKeyOnFloor[enemyKeysDropped] = true;
                droppedKeyX[enemyKeysDropped] = e.x;
                droppedKeyY[enemyKeysDropped] = floorYFromIndex(level2FloorIndex(e.y));
                enemyKeysDropped++;
            }
            if (enemiesKilled == 10) { tenKillDecision = true; tenKillTimer = 5.0; cylinderHitCount = 0; smokeReleased = false; }
        }
    }
}

void updateLevel2Systems()
{
    if (currentLevel != 2) return;
    updatePlayerAutoHeal(); collectLevel2Keys();
    updateLevel2Exit();
    if (level2ExitActivated) return;
    if (smokeTimer > 0.0) { countDown(smokeTimer); if (smokeTimer <= 0.0) smokeReleased = false; }
    int shooters = 0;
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++)
    {
        Enemy& e = level2WaveEnemies[i];
        if (!e.active || !e.alive) continue;
        moveLevel2Enemy(e);
        bool canShoot = enemySeesPlayer(e) && shooters < 2;
        if (canShoot) shooters++;
        updateEnemy(e, canShoot);
    }
    recordLevel2Deaths();
    // At 10 kills, the player has five seconds to activate the gas cylinder.
    // Successful smoke changes subsequent waves from 2 to 3 enemies.
    if (tenKillDecision)
    {
        countDown(tenKillTimer);
        if (tenKillTimer <= 0.0)
        {
            tenKillDecision = false;
        }
        return;
    }

    int groupSize = threeEnemySmokeMode ? 3 : 2;
    // A group must be entirely cleared before the next group appears.
    if (livingLevel2Enemies() == 0 && killsSinceReplacement >= groupSize &&
        enemiesSpawned < LEVEL2_TOTAL_ENEMIES)
    {
        killsSinceReplacement -= groupSize;
        spawnLevel2ReplacementGroup(groupSize);
    }

    // Replacements are created only by the kill-count rule above. This
    // intentionally prevents any extra enemies from appearing on their own.
}

void drawLevel2Cylinder()
{
    int x = (int)worldToScreen(LEVEL2_CYLINDER_X), y = (int)LEVEL2_CYLINDER_Y;
    iSetColor(150, 38, 30); iFilledRectangle(x - 17, y, 34, 70);
    iSetColor(215, 70, 50); iFilledEllipse(x, y + 70, 17, 10);
    iSetColor(230, 210, 170); iText(x - 9, y + 30, "GAS", GLUT_BITMAP_HELVETICA_12);
}

void drawLevel2Smoke()
{
    if (!smokeReleased || smokeCloudTexture == 0) return;
    for (int i = 0; i < SMOKE_CLOUD_COUNT; i++) iShowImage((int)worldToScreen(smokeWorldX[i]) - 85, (int)LEVEL2_FLOOR_1_Y, 170, 90, smokeCloudTexture);
}

void drawLevel2Systems()
{
    drawLevel2Cylinder(); drawLevel2Smoke();
    if (!mannequinBroken) iShowImage((int)worldToScreen(LEVEL2_MANNEQUIN_X) - 28, (int)LEVEL2_MANNEQUIN_Y, 56, 110, mannequinTexture);
    if (mannequinKeyOnFloor) iShowImage((int)worldToScreen(LEVEL2_MANNEQUIN_X) - 24, (int)LEVEL2_MANNEQUIN_Y, 48, 30, keyTexture);
    for (int i = 0; i < 4; i++) if (droppedKeyOnFloor[i]) iShowImage((int)worldToScreen(droppedKeyX[i]) - 24, (int)droppedKeyY[i], 48, 30, keyTexture);
    for (int i = 0; i < LEVEL2_WAVE_SIZE; i++) drawEnemy(level2WaveEnemies[i]);

    if (level2ExitUnlocked())
    {
        iSetColor(255, 230, 70);
        iText((int)worldToScreen(LEVEL2_EXIT_X) - 65, (int)LEVEL2_FLOOR_1_Y + 100,
              "EXIT TO LEVEL 3", GLUT_BITMAP_HELVETICA_18);
        iText((int)worldToScreen(LEVEL2_EXIT_X) - 76, (int)LEVEL2_FLOOR_1_Y + 78,
              "Stand here for 2 seconds", GLUT_BITMAP_HELVETICA_12);
    }
}

#endif
