#ifndef LEVEL2_SUPPORT_H
#define LEVEL2_SUPPORT_H

#include "Config.h"
#include "Player.h"

// The Level 2 wave system uses these floor boundaries to keep
// enemies and shots on the same building floor.
inline int level2FloorIndex(double y)
{
    if (y >= (LEVEL2_FLOOR_3_Y + LEVEL2_FLOOR_4_Y) / 2.0) return 4;
    if (y >= (LEVEL2_FLOOR_2_Y + LEVEL2_FLOOR_3_Y) / 2.0) return 3;
    if (y >= (LEVEL2_FLOOR_1_Y + LEVEL2_FLOOR_2_Y) / 2.0) return 2;
    return 1;
}

// Level 2 retains its original delayed health regeneration: after
// five seconds without taking damage, restore ten HP every two seconds.
double level2SecondsSincePlayerHit = 0.0;
double level2PlayerHealTickTimer = 0.0;
int level2ObservedPlayerHP = PLAYER_MAX_HP;

void resetLevel2AutoHeal()
{
    level2SecondsSincePlayerHit = 0.0;
    level2PlayerHealTickTimer = 0.0;
    level2ObservedPlayerHP = playerHP;
}

void updatePlayerAutoHeal()
{
    if (playerHP < level2ObservedPlayerHP)
    {
        level2SecondsSincePlayerHit = 0.0;
        level2PlayerHealTickTimer = 0.0;
    }
    level2ObservedPlayerHP = playerHP;
    level2SecondsSincePlayerHit += TICK;
    if (level2SecondsSincePlayerHit < 5.0 || playerHP >= PLAYER_MAX_HP) return;

    level2PlayerHealTickTimer += TICK;
    if (level2PlayerHealTickTimer >= 2.0)
    {
        level2PlayerHealTickTimer = 0.0;
        playerHP += 10;
        if (playerHP > PLAYER_MAX_HP) playerHP = PLAYER_MAX_HP;
        level2ObservedPlayerHP = playerHP;
    }
}

#endif
