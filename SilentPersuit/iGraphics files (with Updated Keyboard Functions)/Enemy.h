#ifndef ENEMY_H
#define ENEMY_H

// =========================================================
// SILENT PURSUIT - Enemy.h
// =========================================================

#include "Config.h"
#include "Draw.h"
#include "Assets.h"
#include "World.h"
#include "Effects.h"
#include "Player.h"

// ---------------------------------------------------------
// 1. ENEMY NUMBERS
// ---------------------------------------------------------
const int ENEMY_MAX_HP = 50;
const int ENEMY_DAMAGE_PER_HIT = 10;

const int PLAYER_BODY_DAMAGE = 10;
const int PLAYER_HEADSHOT_DAMAGE = ENEMY_MAX_HP / 2;

const double ENEMY_FIRE_INTERVAL = 0.5;
const double ENEMY_HIT_FLASH_DURATION = 0.15;
const double ENEMY_DYING_HOLD_TIME = 1.0;
const double ENEMY_RECOIL_DURATION = 0.1;
const double ENEMY_RECOIL_PIXELS = 6.0;
double enemyGunSoundTimer = 0.0;
bool enemyGunSoundPlaying = false;

// ---------------------------------------------------------
// 2. HIT AREAS
// ---------------------------------------------------------
const double HEAD_CENTER_X = 0.63 * SPRITE_DRAW_W;
const double HEAD_CENTER_Y = 0.845 * SPRITE_DRAW_H;
const double HEAD_RADIUS = 0.090 * SPRITE_DRAW_H;

const double BODY_LEFT = 0.33 * SPRITE_DRAW_W;
const double BODY_RIGHT = 0.87 * SPRITE_DRAW_W;
const double BODY_BOTTOM = 0.05 * SPRITE_DRAW_H;
const double BODY_TOP = 0.80 * SPRITE_DRAW_H;

const int HIT_NOTHING = 0;
const int HIT_BODY = 1;
const int HIT_HEAD = 2;

bool showHitBoxes = false;

// ---------------------------------------------------------
// 3. ENEMY POSITIONS (Level 1)
// ---------------------------------------------------------
const double ENEMY1_X = ENEMY_BLOCK_X + 25.0;
const double ENEMY2_X = OBSTACLE_X1 + 140.0;
const double ENEMY1_Y_OFFSET = -115.0;
const double ENEMY2_Y_OFFSET = -95.0;

// ---------------------------------------------------------
// 4. THE LEVEL 1 GUARD STRUCT
// ---------------------------------------------------------
struct Enemy
{
    double x, y;
    double dy;
    int hp;
    bool active;
    bool alive;
    bool facingRight;
    double dyingTimer;
    bool corpseGone;
    double hitFlashTimer;
    double fireCooldownTimer;
    double muzzleFlashTimer;
    double recoilTimer;
    bool fallsOffLedge;
    bool usingStairs;
    double stairStartX;
    double stairEndX;
    int stairStartFloor;
    int stairEndFloor;
    unsigned int idleTex;
    unsigned int hitTex;
    unsigned int fallTex;
    unsigned int deadTex;
};

Enemy enemy1;
Enemy enemy2;

const int ENEMY_COUNT = 2;
Enemy* allEnemies[ENEMY_COUNT] = { &enemy1, &enemy2 };

// Level 2 keeps its wave enemies separate from Level 1 guard state.
const int LEVEL2_WAVE_SIZE = 4;
Enemy level2WaveEnemies[LEVEL2_WAVE_SIZE];

// ---------------------------------------------------------
// 5. WHERE THE SPRITE IS DRAWN
// ---------------------------------------------------------
double enemySpriteScale() { return currentLevel == 2 ? 0.55 : 1.0; }
double enemySpriteW() { return SPRITE_DRAW_W * enemySpriteScale(); }
double enemySpriteH() { return SPRITE_DRAW_H * enemySpriteScale(); }
double enemyDrawX(Enemy& e) { return worldToScreen(e.x) - enemySpriteW() / 2.0; }
double enemyDrawY(Enemy& e)
{
    double scale = enemySpriteScale();
    return e.y + SPRITE_Y_OFFSET * scale + (currentLevel == 2 ? 8.0 : 0.0);
}

void screenPointToSprite(Enemy& e, double screenPX, double screenPY, double& outU, double& outV)
{
    double centerX = worldToScreen(e.x);
    double x = mirrorX(screenPX, centerX, e.facingRight);
    outU = x - enemyDrawX(e);
    outV = screenPY - enemyDrawY(e);
}

int enemyHitZone(Enemy& e, double screenPX, double screenPY)
{
    if (!e.active || !e.alive) return HIT_NOTHING;
    double u, v;
    screenPointToSprite(e, screenPX, screenPY, u, v);
    double scale = enemySpriteScale();
    if (distance2D(u, v, HEAD_CENTER_X * scale, HEAD_CENTER_Y * scale) <= HEAD_RADIUS * scale) return HIT_HEAD;
    if (u >= BODY_LEFT * scale && u <= BODY_RIGHT * scale && v >= BODY_BOTTOM * scale && v <= BODY_TOP * scale) return HIT_BODY;
    return HIT_NOTHING;
}

void damageEnemy(Enemy& e, int amount)
{
    e.hp -= amount;
    if (e.hp < 0) e.hp = 0;
    e.hitFlashTimer = ENEMY_HIT_FLASH_DURATION;
}

// ---------------------------------------------------------
// 6. DRAWING LEVEL 1 GUARDS
// ---------------------------------------------------------
void drawEnemyHpBar(double screenX, double topScreenY, int hp, int maxHp)
{
    int barW = 60, barH = 8;
    int bx = (int)(screenX - barW / 2.0);
    int by = (int)(topScreenY + 10);
    iSetColor(20, 20, 20);
    iFilledRectangle(bx - 2, by - 2, barW + 4, barH + 4);
    iSetColor(60, 60, 60);
    iFilledRectangle(bx, by, barW, barH);
    double hpFrac = clampD((double)hp / (double)maxHp, 0.0, 1.0);
    iSetColor(210, 40, 30);
    iFilledRectangle(bx, by, (int)(barW * hpFrac), barH);
}

void drawEnemyHitBoxes(Enemy& e)
{
    if (!e.active || !e.alive) return;
    double scale = enemySpriteScale();
    double centerX = worldToScreen(e.x);
    double baseX = enemyDrawX(e);
    double baseY = enemyDrawY(e);
    double headX = mirrorX(baseX + HEAD_CENTER_X * scale, centerX, e.facingRight);
    double headY = baseY + HEAD_CENTER_Y * scale;
    iSetColor(255, 230, 40);
    iCircle((int)headX, (int)headY, (int)(HEAD_RADIUS * scale));
    double leftX = mirrorX(baseX + BODY_LEFT * scale, centerX, e.facingRight);
    double rightX = mirrorX(baseX + BODY_RIGHT * scale, centerX, e.facingRight);
    if (leftX > rightX) { double tmp = leftX; leftX = rightX; rightX = tmp; }
    iSetColor(255, 80, 60);
    iRectangle((int)leftX, (int)(baseY + BODY_BOTTOM * scale), (int)(rightX - leftX), (int)((BODY_TOP - BODY_BOTTOM) * scale));
}

void drawEnemy(Enemy& e)
{
    if (!e.active || e.corpseGone) return;
    unsigned int tex = e.idleTex;
    if (!e.alive)
    {
        if (e.fallsOffLedge) tex = e.fallTex;
        else if (e.fallTex != 0 && e.dyingTimer < 0.4) tex = e.fallTex;
        else if (e.deadTex != 0) tex = e.deadTex;
        else tex = e.fallTex;
    }
    else if (e.hitFlashTimer > 0.0 && e.hitTex != 0) tex = e.hitTex;

    if (tex == 0) return;

    double recoilOffset = 0.0;
    if (e.recoilTimer > 0.0)
    {
        double t = e.recoilTimer / ENEMY_RECOIL_DURATION;
        recoilOffset = ENEMY_RECOIL_PIXELS * t * (e.facingRight ? -1.0 : 1.0);
    }

    double centerX = worldToScreen(e.x);
    double drawX = enemyDrawX(e) + recoilOffset;
    double drawY = enemyDrawY(e);
    double drawW = enemySpriteW();
    double drawH = enemySpriteH();

    drawImageFlipped(drawX, drawY, (int)drawW, (int)drawH, tex, e.facingRight, centerX);

    if (!e.alive) return;

    if (e.muzzleFlashTimer > 0.0)
    {
        double flashX = mirrorX(drawX + drawW * 0.12, centerX, e.facingRight);
        double flashY = drawY + drawH * 0.55;
        drawMuzzleFlash(flashX, flashY, e.muzzleFlashTimer);
    }
    drawEnemyHpBar(centerX, drawY + drawH, e.hp, ENEMY_MAX_HP);
    if (showHitBoxes) drawEnemyHitBoxes(e);
}

void updateEnemyAlive(Enemy& e, bool canShoot)
{
    enemyGunSoundPlaying = isGameAudioPlaying("enemy_gun_audio");

    e.facingRight = (assassin.x >= e.x);
    countDown(e.fireCooldownTimer);

    if (canShoot && e.fireCooldownTimer <= 0.0)
    {
        e.fireCooldownTimer = ENEMY_FIRE_INTERVAL;
        e.muzzleFlashTimer = MUZZLE_FLASH_DURATION;
        e.recoilTimer = ENEMY_RECOIL_DURATION;
        playerHP -= ENEMY_DAMAGE_PER_HIT;
        if (state == GAMEPLAY && !enemyGunSoundPlaying)
        {
            playGameAudioChannel("Enemy_Gun_Sound.mp3", "enemy_gun_audio");
            enemyGunSoundPlaying = true;
        }
        if (playerHP < 0) playerHP = 0;
    }
    countDown(e.hitFlashTimer);
    countDown(e.muzzleFlashTimer);
    countDown(e.recoilTimer);

    if (e.hp <= 0)
    {
        e.alive = false;
        e.dyingTimer = 0.0;
        e.dy = 4.0;
        if (!(state == GAMEPLAY && currentLevel == 2))
            playerEP += 20; // Level 2 uses its own timed health regeneration.
    }
}

void updateEnemyDead(Enemy& e)
{
    if (e.fallsOffLedge)
    {
        e.dy -= GRAVITY;
        e.y += e.dy;
        e.x -= 2.5;
        if (e.y < FALL_DEATH_Y)
        {
            e.corpseGone = true;
            e.active = false;
        }
    }
    else
    {
        e.dyingTimer += TICK;
        if (e.dyingTimer >= ENEMY_DYING_HOLD_TIME)
        {
            e.corpseGone = true;
            e.active = false;
        }
    }
}

void updateEnemy(Enemy& e, bool canShoot)
{
    if (!e.active) return;
    if (e.alive) updateEnemyAlive(e, canShoot);
    else         updateEnemyDead(e);
}

void resetEnemy(Enemy& e, double startX, double startY, bool fallsOffLedge)
{
    e.x = startX;
    e.y = startY;
    e.dy = 0.0;
    e.hp = ENEMY_MAX_HP;
    e.active = false;
    e.alive = true;
    e.facingRight = false;
    e.dyingTimer = 0.0;
    e.corpseGone = false;
    e.hitFlashTimer = 0.0;
    e.fireCooldownTimer = ENEMY_FIRE_INTERVAL;
    e.muzzleFlashTimer = 0.0;
    e.recoilTimer = 0.0;
    e.fallsOffLedge = fallsOffLedge;
    e.usingStairs = false;
    e.stairStartX = e.stairEndX = 0.0;
    e.stairStartFloor = e.stairEndFloor = 1;
}

void resetAllEnemies()
{
    resetEnemy(enemy1, ENEMY1_X, groundYAt(ENEMY_BLOCK_X) + ENEMY_BLOCK_H + ENEMY1_Y_OFFSET, true);
    enemy1.idleTex = enemy1IdleTex;
    enemy1.hitTex = enemy1HitTex;
    enemy1.fallTex = enemy1FallTex;
    enemy1.deadTex = 0;

    resetEnemy(enemy2, ENEMY2_X, GROUND_Y_SCENE2 + ENEMY2_Y_OFFSET, false);
    enemy2.idleTex = enemy2IdleTex;
    enemy2.hitTex = 0;
    enemy2.fallTex = enemy2FallTex;
    enemy2.deadTex = enemy2DeadTex;
}

// =========================================================
// 8. LEVEL 3 NINJAS & GUARDS
// =========================================================
const int NINJA_MAX_HP = 50;

enum NinjaType { BLUE_NINJA, RED_NINJA };
enum NinjaState { NINJA_RUNNING, NINJA_ATTACKING, NINJA_DEAD };

struct Ninja
{
    NinjaType type;
    double x, y;
    int hp;
    bool active;

    NinjaState state;
    int animFrame;
    double animTimer;

    bool hasDealtDamageThisSwing;
};

const int MAX_NINJAS = 2;
Ninja ninjas[MAX_NINJAS];
int currentNinjaSpawnIndex = 0;
double nextNinjaSpawnTimer = 0.0;

struct PatrolGuard
{
    double x, y;
    bool active;
    bool alerted;
    bool retreating;
    bool isDead;
    double delayTimer;
    int animFrame;
    double animTimer;
};
PatrolGuard roomGuard;
bool ambushTriggered = false;

void resetNinjas()
{
    for (int i = 0; i < MAX_NINJAS; i++) ninjas[i].active = false;
    currentNinjaSpawnIndex = 0;
    nextNinjaSpawnTimer = 0.0;
}

void spawnNinja(NinjaType type, double startX)
{
    if (currentNinjaSpawnIndex >= MAX_NINJAS) return;

    Ninja& n = ninjas[currentNinjaSpawnIndex];
    n.type = type;
    n.x = startX;
    n.y = GROUND_Y;
    n.hp = NINJA_MAX_HP;
    n.active = true;
    n.state = NINJA_RUNNING;
    n.animFrame = 0;
    n.animTimer = 0.0;
    n.hasDealtDamageThisSwing = false;

    currentNinjaSpawnIndex++;
}

// =========================================================
// 9. LEVEL 4 - HAUNTED MANSION / IMPERIAL PALACE ENEMIES
// =========================================================

// ---------- 9a. ENTRANCE - katana-wielding foe ----------
const int KATANA_FOE_MAX_HP = 50;
const double KATANA_FOE_PATROL_SPEED = 1.2;   
const int KATANA_FOE_DAMAGE = 20;
const double KATANA_FOE_NOTICE_RANGE = 90.0;  
const double KATANA_FOE_ATTACK_COOLDOWN = 1.0;
const double KATANA_FOE_PATROL_MIN_X = 60.0;
const double KATANA_FOE_PATROL_MAX_X = WIN_W - 60.0;

struct KatanaFoe
{
    double x, y;
    int hp;
    bool active;
    bool alive;
    int animFrame;
    double animTimer;
    double attackCooldown;
    int patrolDir;   
};
KatanaFoe katanaFoe;

void resetKatanaFoe(double startX, double startY)
{
    katanaFoe.x = startX;
    katanaFoe.y = startY;
    katanaFoe.hp = KATANA_FOE_MAX_HP;
    katanaFoe.active = true;
    katanaFoe.alive = true;
    katanaFoe.animFrame = 0;
    katanaFoe.animTimer = 0.0;
    katanaFoe.attackCooldown = 0.5;
    katanaFoe.patrolDir = (rand() % 2 == 0) ? 1 : -1;
}

void updateKatanaFoe()
{
    if (!katanaFoe.active || !katanaFoe.alive) return;

    bool playerIsClose = fabs(katanaFoe.x - assassin.x) < KATANA_FOE_NOTICE_RANGE;

    if (playerIsClose && !isHiding)
    {
        countDown(katanaFoe.attackCooldown);
        if (katanaFoe.attackCooldown <= 0.0)
        {
            playerHP -= KATANA_FOE_DAMAGE;
            if (playerHP < 0) playerHP = 0;
            katanaFoe.attackCooldown = KATANA_FOE_ATTACK_COOLDOWN;
        }
    }
    else if (!isHiding)
    {
        if (katanaFoe.x < assassin.x) katanaFoe.x += KATANA_FOE_PATROL_SPEED;
        else                          katanaFoe.x -= KATANA_FOE_PATROL_SPEED;
        katanaFoe.x = clampD(katanaFoe.x, KATANA_FOE_PATROL_MIN_X, KATANA_FOE_PATROL_MAX_X);
        countDown(katanaFoe.attackCooldown);
    }
    else
    {
        countDown(katanaFoe.attackCooldown);
    }

    katanaFoe.animTimer += TICK;
    if (katanaFoe.animTimer > 0.08)
    {
        katanaFoe.animTimer = 0.0;
        katanaFoe.animFrame = (katanaFoe.animFrame + 1) % NINJA_RUN_FRAME_COUNT;
    }
}

void damageKatanaFoe(int amount)
{
    if (!katanaFoe.alive) return;
    katanaFoe.hp -= amount;
    if (katanaFoe.hp <= 0) { 
        katanaFoe.hp = 0; 
        katanaFoe.alive = false; 
        playerEP += 20; // NEW: Grant EP on kill
    }
}

// ---------- 9b. DINING ROOM - ranged maid ----------
const int MAID_MAX_HP = 80;
const double MAID_FIRE_INTERVAL = 1.4;
const double MAID_PROJECTILE_SPEED = 4.0;
const int MAID_PROJECTILE_DAMAGE = 15;
const double MAID_CAST_POSE_TIME = 0.25;

struct MaidProjectile { double x, y; double vx; bool active; };
const int MAID_MAX_PROJECTILES = 6;
MaidProjectile maidProjectiles[MAID_MAX_PROJECTILES];

struct Maid
{
    double x, y;
    int hp;
    bool active;
    bool alive;
    double fireCooldown;
    double castPoseTimer;
    int animFrame;
    double animTimer;
};
Maid maid;

void resetMaid(double startX, double startY)
{
    maid.x = startX;
    maid.y = startY;
    maid.hp = MAID_MAX_HP;
    maid.active = true;
    maid.alive = true;
    maid.fireCooldown = MAID_FIRE_INTERVAL;
    maid.castPoseTimer = 0.0;
    maid.animFrame = 0;
    maid.animTimer = 0.0;
    for (int i = 0; i < MAID_MAX_PROJECTILES; i++) maidProjectiles[i].active = false;
}

void fireMaidProjectile()
{
    for (int i = 0; i < MAID_MAX_PROJECTILES; i++)
    {
        if (!maidProjectiles[i].active)
        {
            maidProjectiles[i].active = true;
            maidProjectiles[i].x = maid.x;
            maidProjectiles[i].y = maid.y + 70.0;
            maidProjectiles[i].vx = (assassin.x < maid.x) ? -MAID_PROJECTILE_SPEED : MAID_PROJECTILE_SPEED;
            return;
        }
    }
}

void updateMaid()
{
    if (maid.active && maid.alive)
    {
        maid.animTimer += TICK;
        if (maid.animTimer > 0.4)
        {
            maid.animTimer = 0.0;
            maid.animFrame = (maid.animFrame + 1) % L4_ENEMY_WALK_FRAME_COUNT;
        }

        countDown(maid.fireCooldown);
        countDown(maid.castPoseTimer);
        if (maid.fireCooldown <= 0.0)
        {
            fireMaidProjectile();
            maid.castPoseTimer = MAID_CAST_POSE_TIME;
            maid.fireCooldown = MAID_FIRE_INTERVAL;
        }
    }

    for (int i = 0; i < MAID_MAX_PROJECTILES; i++)
    {
        if (!maidProjectiles[i].active) continue;
        maidProjectiles[i].x += maidProjectiles[i].vx;

        if (fabs(maidProjectiles[i].x - assassin.x) < 25.0 && fabs(maidProjectiles[i].y - (assassin.y + 90.0)) < 70.0)
        {
            playerHP -= MAID_PROJECTILE_DAMAGE;
            if (playerHP < 0) playerHP = 0;
            maidProjectiles[i].active = false;
            continue;
        }
        if (maidProjectiles[i].x < -50.0 || maidProjectiles[i].x > WIN_W + 50.0) maidProjectiles[i].active = false;
    }
}

void damageMaid(int amount)
{
    if (!maid.alive) return;
    maid.hp -= amount;
    if (maid.hp <= 0) { 
        maid.hp = 0; 
        maid.alive = false; 
        playerEP += 20; // NEW: Grant EP on kill
    }
}

// ---------- 9c. HALLWAY - melee foe ----------
const int HALLWAY_FOE_MAX_HP = 45;
const double HALLWAY_FOE_PATROL_SPEED = 1.3;   
const int HALLWAY_FOE_DAMAGE = 15;
const double HALLWAY_FOE_NOTICE_RANGE = 60.0;
const double HALLWAY_FOE_ATTACK_COOLDOWN = 1.0;
const double HALLWAY_FOE_PATROL_MIN_X = 60.0;
const double HALLWAY_FOE_PATROL_MAX_X = WIN_W - 60.0;

struct HallwayFoe
{
    double x, y;
    int hp;
    bool active;
    bool alive;
    int animFrame;
    double animTimer;
    double attackCooldown;
    int patrolDir;   
};
HallwayFoe hallwayFoe;

void resetHallwayFoe(double startX, double startY)
{
    hallwayFoe.x = startX;
    hallwayFoe.y = startY;
    hallwayFoe.hp = HALLWAY_FOE_MAX_HP;
    hallwayFoe.active = true;
    hallwayFoe.alive = true;
    hallwayFoe.animFrame = 0;
    hallwayFoe.animTimer = 0.0;
    hallwayFoe.attackCooldown = 0.5;
    hallwayFoe.patrolDir = (rand() % 2 == 0) ? 1 : -1;
}

void updateHallwayFoe()
{
    if (!hallwayFoe.active || !hallwayFoe.alive) return;

    bool playerIsClose = fabs(hallwayFoe.x - assassin.x) < HALLWAY_FOE_NOTICE_RANGE;

    if (playerIsClose && !isHiding)
    {
        countDown(hallwayFoe.attackCooldown);
        if (hallwayFoe.attackCooldown <= 0.0)
        {
            playerHP -= HALLWAY_FOE_DAMAGE;
            if (playerHP < 0) playerHP = 0;
            hallwayFoe.attackCooldown = HALLWAY_FOE_ATTACK_COOLDOWN;
        }
    }
    else if (!isHiding)
    {
        if (hallwayFoe.x < assassin.x) hallwayFoe.x += HALLWAY_FOE_PATROL_SPEED;
        else                           hallwayFoe.x -= HALLWAY_FOE_PATROL_SPEED;
        hallwayFoe.x = clampD(hallwayFoe.x, HALLWAY_FOE_PATROL_MIN_X, HALLWAY_FOE_PATROL_MAX_X);
        countDown(hallwayFoe.attackCooldown);
    }
    else
    {
        countDown(hallwayFoe.attackCooldown);
    }

    hallwayFoe.animTimer += TICK;
    if (hallwayFoe.animTimer > 0.12)
    {
        hallwayFoe.animTimer = 0.0;
        hallwayFoe.animFrame = (hallwayFoe.animFrame + 1) % HALLWAY_FOE_FRAME_COUNT;
    }
}

void damageHallwayFoe(int amount)
{
    if (!hallwayFoe.alive) return;
    hallwayFoe.hp -= amount;
    if (hallwayFoe.hp <= 0) { 
        hallwayFoe.hp = 0; 
        hallwayFoe.alive = false; 
        playerEP += 20; // NEW: Grant EP on kill
    }
}

// ---------- 9d. LIBRARY - spider ----------
const int SPIDER_MAX_HP = 40;
const double SPIDER_PATROL_SPEED = 1.4;   
const int SPIDER_CONTACT_DAMAGE = 15;
const double SPIDER_NOTICE_RANGE = 55.0;
const double SPIDER_ATTACK_COOLDOWN = 1.0;
const double SPIDER_PATROL_MIN_X = 60.0;
const double SPIDER_PATROL_MAX_X = WIN_W - 60.0;

struct Spider
{
    double x, y;
    int hp;
    bool active;
    bool alive;
    int animFrame;
    double animTimer;
    double attackCooldown;
    int patrolDir;   
};
Spider spider;

void resetSpider(double startX, double startY)
{
    spider.x = startX;
    spider.y = startY;
    spider.hp = SPIDER_MAX_HP;
    spider.active = true;
    spider.alive = true;
    spider.animFrame = 0;
    spider.animTimer = 0.0;
    spider.attackCooldown = 0.5;
    spider.patrolDir = (rand() % 2 == 0) ? 1 : -1;
}

void updateSpider()
{
    if (!spider.active || !spider.alive) return;

    bool playerIsClose = fabs(spider.x - assassin.x) < SPIDER_NOTICE_RANGE;

    if (playerIsClose && !isHiding)
    {
        countDown(spider.attackCooldown);
        if (spider.attackCooldown <= 0.0)
        {
            playerHP -= SPIDER_CONTACT_DAMAGE;
            if (playerHP < 0) playerHP = 0;
            spider.attackCooldown = SPIDER_ATTACK_COOLDOWN;
        }
    }
    else if (!isHiding)
    {
        spider.x += (spider.x < assassin.x) ? SPIDER_PATROL_SPEED : -SPIDER_PATROL_SPEED;
        spider.x = clampD(spider.x, SPIDER_PATROL_MIN_X, SPIDER_PATROL_MAX_X);
        countDown(spider.attackCooldown);
    }
    else
    {
        countDown(spider.attackCooldown);
    }

    spider.animTimer += TICK;
    if (spider.animTimer > 0.1)
    {
        spider.animTimer = 0.0;
        spider.animFrame = (spider.animFrame + 1) % L4_ENEMY_WALK_FRAME_COUNT;
    }
}

void damageSpider(int amount)
{
    if (!spider.alive) return;
    spider.hp -= amount;
    if (spider.hp <= 0) { 
        spider.hp = 0; 
        spider.alive = false; 
        playerEP += 20; // NEW: Grant EP on kill
    }
}

// ---------- 9e. IMPERIAL THRONE - final boss ----------
const int BOSS_MAX_HP = 400;
const double BOSS_SPEED = 2.2;              
const int BOSS_ATTACK_DAMAGE = 30;
const double BOSS_ATTACK_RANGE = 100.0;
const double BOSS_ATTACK_WINDUP = 0.25;     
const double BOSS_ATTACK_RECOVER = 0.5;     
const double BOSS_ANIM_FRAME_TIME = 0.15;   

enum BossState { BOSS_WALK, BOSS_WINDUP, BOSS_RECOVER, BOSS_DEAD };

struct Boss
{
    double x, y;
    int hp;
    bool active;
    BossState bState;
    double stateTimer;
    int animFrame;
    double animTimer;
};
Boss boss;

void resetBoss(double startX, double startY)
{
    boss.x = startX;
    boss.y = startY;
    boss.hp = BOSS_MAX_HP;
    boss.active = true;
    boss.bState = BOSS_WALK;
    boss.stateTimer = 0.0;
    boss.animFrame = 0;
    boss.animTimer = 0.0;
}

void updateBoss()
{
    if (!boss.active || boss.bState == BOSS_DEAD) return;

    double dist = fabs(boss.x - assassin.x);

    if (boss.bState == BOSS_WALK)
    {
        boss.animTimer += TICK;
        if (boss.animTimer > BOSS_ANIM_FRAME_TIME)
        {
            boss.animTimer = 0.0;
            boss.animFrame = (boss.animFrame + 1) % BOSS_WALK_FRAME_COUNT;
        }

        if (dist > BOSS_ATTACK_RANGE)
        {
            if (boss.x < assassin.x) boss.x += BOSS_SPEED;
            else                     boss.x -= BOSS_SPEED;
        }
        else
        {
            boss.bState = BOSS_WINDUP;
            boss.stateTimer = 0.0;
            boss.animFrame = 0;   
            boss.animTimer = 0.0;
        }
    }
    else if (boss.bState == BOSS_WINDUP)
    {
        boss.stateTimer += TICK;
        if (boss.stateTimer >= BOSS_ATTACK_WINDUP)
        {
            if (fabs(boss.x - assassin.x) <= BOSS_ATTACK_RANGE + 20.0)
            {
                playerHP -= BOSS_ATTACK_DAMAGE;
                if (playerHP < 0) playerHP = 0;
            }
            boss.bState = BOSS_RECOVER;
            boss.stateTimer = 0.0;
        }
    }
    else if (boss.bState == BOSS_RECOVER)
    {
        boss.stateTimer += TICK;
        if (boss.stateTimer >= BOSS_ATTACK_RECOVER)
        {
            boss.bState = BOSS_WALK;
            boss.stateTimer = 0.0;
        }
    }
}

void damageBoss(int amount)
{
    if (boss.bState == BOSS_DEAD) return;
    boss.hp -= amount;
    if (boss.hp <= 0) { 
        boss.hp = 0; 
        boss.bState = BOSS_DEAD; 
        playGameAudio("BOSS_ENEMY_SOUND.mp3");
        playerEP += 20; // NEW: Grant EP on kill
    }
}

// ---------- 9f. ARMORY - patrolling halberd guard ----------
const int ARMORY_GUARD_MAX_HP = 100;
const double ARMORY_GUARD_PATROL_SPEED = 1.5;
const int ARMORY_GUARD_DAMAGE = 25;
const double ARMORY_GUARD_NOTICE_RANGE = 130.0;  
const double ARMORY_GUARD_ATTACK_COOLDOWN = 1.0;
const double ARMORY_GUARD_PATROL_MIN_X = 60.0;
const double ARMORY_GUARD_PATROL_MAX_X = WIN_W - 60.0;

struct ArmoryGuardType
{
    double x, y;
    int hp;
    bool active;
    bool alive;
    bool facingRight;   
    int animFrame;
    double animTimer;
    double attackCooldown;
};
ArmoryGuardType armoryGuard;

void resetArmoryGuard(double startX, double startY)
{
    armoryGuard.x = startX;
    armoryGuard.y = startY;
    armoryGuard.hp = ARMORY_GUARD_MAX_HP;
    armoryGuard.active = true;
    armoryGuard.alive = true;
    armoryGuard.facingRight = true;   
    armoryGuard.animFrame = 0;
    armoryGuard.animTimer = 0.0;
    armoryGuard.attackCooldown = 0.5;
}

void updateArmoryGuard()
{
    if (!armoryGuard.active || !armoryGuard.alive) return;

    double step = ARMORY_GUARD_PATROL_SPEED * (armoryGuard.facingRight ? 1.0 : -1.0);
    armoryGuard.x += step;
    if (armoryGuard.x >= ARMORY_GUARD_PATROL_MAX_X) { armoryGuard.x = ARMORY_GUARD_PATROL_MAX_X; armoryGuard.facingRight = false; }
    if (armoryGuard.x <= ARMORY_GUARD_PATROL_MIN_X) { armoryGuard.x = ARMORY_GUARD_PATROL_MIN_X; armoryGuard.facingRight = true; }

    bool playerIsClose = fabs(armoryGuard.x - assassin.x) < ARMORY_GUARD_NOTICE_RANGE;

    if (playerIsClose && !isHiding)
    {
        armoryGuard.facingRight = (assassin.x >= armoryGuard.x);

        countDown(armoryGuard.attackCooldown);
        if (armoryGuard.attackCooldown <= 0.0)
        {
            playerHP -= ARMORY_GUARD_DAMAGE;
            if (playerHP < 0) playerHP = 0;
            armoryGuard.attackCooldown = ARMORY_GUARD_ATTACK_COOLDOWN;
        }
    }
    else
    {
        countDown(armoryGuard.attackCooldown);
    }

    armoryGuard.animTimer += TICK;
    if (armoryGuard.animTimer > 0.18)
    {
        armoryGuard.animTimer = 0.0;
        armoryGuard.animFrame = (armoryGuard.animFrame + 1) % GUARD_FRAME_COUNT;
    }
}

void damageArmoryGuard(int amount)
{
    if (!armoryGuard.alive) return;

    bool playerBehind = armoryGuard.facingRight
        ? (assassin.x < armoryGuard.x)    
        : (assassin.x > armoryGuard.x);   

    if (playerBehind)
    {
        armoryGuard.hp = 0;
        armoryGuard.alive = false;
        playerEP += 20; // NEW: Grant EP on kill
        return;
    }

    armoryGuard.hp -= amount;
    if (armoryGuard.hp <= 0) { 
        armoryGuard.hp = 0; 
        armoryGuard.alive = false; 
        playerEP += 20; // NEW: Grant EP on kill
    }
}

#endif