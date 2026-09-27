#ifndef CONFIG_H
#define CONFIG_H

// =========================================================
// SILENT PURSUIT - Config.h
// =========================================================

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------
// 1. GAME STATES
// ---------------------------------------------------------
enum GameState
{
    LOADING,
    MENU,
    LEVEL_SELECT,
    OPTIONS,
    CREDITS,
    GAMEPLAY,
    GAME_OVER,
    LEVEL3_INTRO,
    LEVEL3,
    LEVEL3_INTERIOR,
    LEVEL3_TRANSITION,
    LEVEL3_INTERIOR_2,
    LEVEL3_TIGER,
    LEVEL3_VAULT_INTRO, // NEW: Cinematic door opening sequence
    LEVEL3_VAULT,       // NEW: The final vault room
    LEVEL4              // NEW: Haunted mansion - room-by-room background progression
};

GameState state = LOADING;
int currentLevel = 1;

// ---------------------------------------------------------
// 2. TIMING
// ---------------------------------------------------------
const int  TIMER_INTERVAL_MS = 20;
const double TICK = 0.02;
double animTime = 0.0;
bool movementAKeyDown = false;
bool movementDKeyDown = false;
bool runningAudioPlaying = false;
bool loadingAudioPlaying = false;

// ---------------------------------------------------------
// 3. WINDOW & WORLD SIZE
// ---------------------------------------------------------
const int WIN_W = 800;
const int WIN_H = 600;
const double WORLD_WIDTH = WIN_W * 2.0;
const int BUILDING_TILE_COUNT = 4;
const double LEVEL2_WORLD_WIDTH = WIN_W * BUILDING_TILE_COUNT;

// ---------------------------------------------------------
// 4. PHYSICS & GROUND
// ---------------------------------------------------------
const double GRAVITY = 0.6;
const double GROUND_Y = 100.0;
const double GROUND_Y_SCENE2 = 155.0;

// Level 2: floors and stair lanes in the four-tile building.
const double LEVEL2_FLOOR_1_Y = 15.0;
const double LEVEL2_FLOOR_2_Y = 163.0;
const double LEVEL2_FLOOR_3_Y = 299.0;
const double LEVEL2_FLOOR_4_Y = 436.0;
const double STAIR1_X0 = 45.0;
const double STAIR1_X1 = 285.0;
const double STAIR2_X0 = 1210.0;
const double STAIR2_X1 = 1530.0;
const double STAIR3_X0 = 2720.0;
const double STAIR3_X1 = 3040.0;
const double LEVEL2_CEILING_CLEARANCE = 136.0;
const double GAP_START_X = 340.0;
const double GAP_END_X = 470.0;
const double FALL_DEATH_Y = -250.0;

// ---------------------------------------------------------
// 5. LEVEL 1 OBJECT POSITIONS
// ---------------------------------------------------------
const double OBSTACLE_X0 = WIN_W + 100.0;
const double OBSTACLE_X1 = WIN_W + 300.0;
const double OBSTACLE_TOP_Y = GROUND_Y_SCENE2 + 95.0;
const double OBSTACLE_PLATEAU_MARGIN = 22.0;

const double ENEMY_BLOCK_X = 850.0;
const double ENEMY_BLOCK_W = 80.0;
const double ENEMY_BLOCK_H = 65.0;

const double SWITCH_X = 1450.0;
const double MINE_X = 1520.0;

// ---------------------------------------------------------
// 6. COLORS
// ---------------------------------------------------------
const int CLR_BTN_TEXT_R = 200;
const int CLR_BTN_TEXT_G = 200;
const int CLR_BTN_TEXT_B = 200;

const int CLR_GLASS_R = 190;
const int CLR_GLASS_G = 115;
const int CLR_GLASS_B = 40;

const int CLR_TEXT_R = 250;
const int CLR_TEXT_G = 235;
const int CLR_TEXT_B = 210;

// ---------------------------------------------------------
// 7. SMALL HELPERS
// ---------------------------------------------------------
inline void countDown(double& timer) { if (timer > 0.0) { timer -= TICK; if (timer < 0.0) timer = 0.0; } }
inline double clampD(double value, double lo, double hi) { if (value < lo) return lo; if (value > hi) return hi; return value; }
inline int clampI(int value, int lo, int hi) { if (value < lo) return lo; if (value > hi) return hi; return value; }
inline double distance2D(double x1, double y1, double x2, double y2) { double dx = x2 - x1; double dy = y2 - y1; return sqrt(dx * dx + dy * dy); }

#endif