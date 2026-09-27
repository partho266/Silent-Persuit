#ifndef UI_H
#define UI_H

// =========================================================
// SILENT PURSUIT - UI.h
// ---------------------------------------------------------
// Menu buttons, the mouse-to-game coordinate conversion, and
// the aiming crosshair.
//
// It also keeps track of whether the mouse is CURRENTLY being
// moved. Player.h uses that to decide whether the mouse or
// the keyboard controls which way the assassin is facing.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"
#include "Draw.h"


// ---------------------------------------------------------
// 1. BUTTON
// ---------------------------------------------------------

struct Button
{
    int x;
    int y;
    int w;
    int h;

    char label[30];

    bool hovered;     // is the mouse on top of it right now
    bool locked;      // greyed out and not clickable
};

// --- shared sizes and positions ---
const int BTN_LEFT_X = 50;
const int BTN_RIGHT_MARGIN = 25;

const int BTN_W = 170;
const int BTN_H = 50;

const int BTN_X = WIN_W - BTN_RIGHT_MARGIN - BTN_W;

// --- main menu ---
Button btnNewGame = { BTN_X, 380, BTN_W, BTN_H, "New Game",  false, false };
Button btnOptions = { BTN_X, 310, BTN_W, BTN_H, "Options",   false, false };
Button btnCredits = { BTN_X, 240, BTN_W, BTN_H, "Credits",   false, false };
Button btnQuit = { BTN_X, 170, BTN_W, BTN_H, "Quit Game", false, false };

// --- options / credits ---
Button btnAudio = { BTN_LEFT_X, 380, BTN_W, BTN_H, "Audio",    false, false };
Button btnControls = { BTN_LEFT_X, 310, BTN_W, BTN_H, "Controls", false, false };
Button btnOptBack = { BTN_LEFT_X, 240, BTN_W, BTN_H, "Back",     false, false };
Button btnCredBack = { BTN_LEFT_X,  80, BTN_W, BTN_H, "Back",     false, false };

// --- game over ---
const int GO_BTN_W = 220;
const int GO_BTN_H = 56;

Button btnRetry = { WIN_W / 2 - GO_BTN_W / 2, 260, GO_BTN_W, GO_BTN_H, "Retry",     false, false };
Button btnGoToMenu = { WIN_W / 2 - GO_BTN_W / 2, 190, GO_BTN_W, GO_BTN_H, "Main Menu", false, false };

// --- level 4 victory screen (Imperial Throne 2 - see Screens.h) ---
Button btnGameCompleted = { WIN_W / 2 - GO_BTN_W / 2, 200, GO_BTN_W, GO_BTN_H, "Game Completed", false, false };

// --- level select ---
const int LVL_BTN_W = 220;
const int LVL_BTN_H = 50;
const int LVL_BTN_X = WIN_W / 2 - LVL_BTN_W / 2;

Button btnLevel1 = { LVL_BTN_X, 400, LVL_BTN_W, LVL_BTN_H, "Level 1",          false, false };
Button btnLevel2 = { LVL_BTN_X, 330, LVL_BTN_W, LVL_BTN_H, "Level 2",          false, false };
Button btnLevel3 = { LVL_BTN_X, 260, LVL_BTN_W, LVL_BTN_H, "Level 3",          false, false };
Button btnLevel4 = { LVL_BTN_X, 190, LVL_BTN_W, LVL_BTN_H, "Level 4",          false, false };
Button btnLvlBack = { BTN_LEFT_X,  80, BTN_W,     BTN_H,    "Back",             false, false };
// btnLevel5 removed - Level 4 now takes its slot in the level-select flow.


// ---------------------------------------------------------
// 2. IS THE MOUSE ON THIS BUTTON?
// ---------------------------------------------------------

bool isInside(Button b, int mx, int my)
{
    return (mx >= b.x && mx <= b.x + b.w &&
        my >= b.y && my <= b.y + b.h);
}


// ---------------------------------------------------------
// 3. DRAWING A BUTTON
// ---------------------------------------------------------
// The button is an eight-sided shape: a rectangle with its
// four corners cut off by BTN_CLIP pixels.
// ---------------------------------------------------------

const double BTN_CLIP = 12.0;

void drawButton(Button b)
{
    double c = BTN_CLIP;

    // corner points, measured from the bottom-left of the button
    double vx[8] = { c, b.w - c, b.w,     b.w,     b.w - c, c,   0,       0 };
    double vy[8] = { 0, 0,       c,       b.h - c, b.h,     b.h, b.h - c, c };

    double px[8];
    double py[8];

    for (int i = 0; i < 8; i++)
    {
        px[i] = b.x + vx[i];
        py[i] = b.y + vy[i];
    }

    // dark see-through glass behind the button
    beginTransparency();
    setColorAlpha(26, 18, 13, (b.hovered && !b.locked) ? 0.65 : 0.50);
    iFilledPolygon(px, py, 8);
    endTransparency();

    // border
    if (b.locked) iSetColor(80, 80, 80);
    else          iSetColor(CLR_GLASS_R, CLR_GLASS_G, CLR_GLASS_B);
    iPolygon(px, py, 8);

    // two thin decorative lines
    if (b.locked) iSetColor(100, 100, 100);
    else          iSetColor((int)(CLR_GLASS_R * 0.85), (int)(CLR_GLASS_G * 0.85), (int)(CLR_GLASS_B * 0.85));

    iLine(b.x + (int)c + 4, b.y + 5, b.x + b.w - (int)c - 4, b.y + 5);
    iLine(b.x + (int)c + 4, b.y + b.h - 5, b.x + b.w - (int)c - 4, b.y + b.h - 5);

    // label
    if (b.locked) iSetColor(120, 120, 120);
    else          iSetColor(230, 210, 175);

    iText(b.x + (int)c + 14, b.y + b.h / 2 - 6, b.label, GLUT_BITMAP_HELVETICA_18);
}


// ---------------------------------------------------------
// 4. MOUSE POSITION -> GAME POSITION
// ---------------------------------------------------------
// The window can be resized, so the raw mouse pixels are
// scaled back to our fixed 800 x 600 game coordinates.
// GLUT also measures Y from the top, we measure it from the
// bottom, so it gets flipped here too.
// ---------------------------------------------------------

void toLogicalCoords(int mx, int my, int& lx, int& ly)
{
    int actualW = glutGet(GLUT_WINDOW_WIDTH);
    int actualH = glutGet(GLUT_WINDOW_HEIGHT);

    if (actualW <= 0) actualW = WIN_W;
    if (actualH <= 0) actualH = WIN_H;

    lx = (int)((double)mx * WIN_W / actualW);
    ly = (int)((double)(actualH - (WIN_H - my)) * WIN_H / actualH);
}


// ---------------------------------------------------------
// 5. CROSSHAIR + "IS THE MOUSE ACTIVE?"
// ---------------------------------------------------------
// mouseActiveTimer is refilled every time the mouse actually
// moves (or fires). While it is above zero we treat the mouse
// as "active" and let it turn the character. When the player
// stops touching the mouse the timer runs out and the
// keyboard takes the facing direction back.
// ---------------------------------------------------------

int crosshairX = WIN_W / 2;
int crosshairY = WIN_H / 2;

const double MOUSE_AIM_HOLD = 1.2;   // seconds the mouse keeps control

double mouseActiveTimer = 0.0;
int lastMouseX = -1;
int lastMouseY = -1;

bool isMouseActive()
{
    return (mouseActiveTimer > 0.0);
}

void keepMouseActive()
{
    mouseActiveTimer = MOUSE_AIM_HOLD;
}

// Called from the mouse callbacks. It only counts as movement
// if the pointer really changed position.
void updateCrosshair(int lx, int ly)
{
    if (lx != lastMouseX || ly != lastMouseY)
    {
        keepMouseActive();
        lastMouseX = lx;
        lastMouseY = ly;
    }

    crosshairX = lx;
    crosshairY = ly;
}

void drawCrosshair()
{
    // Brighter while the mouse is steering, dimmer when the
    // keyboard has taken over, so the player can see which
    // one is in charge.
    if (isMouseActive()) iSetColor(0, 255, 200);
    else                 iSetColor(0, 130, 110);

    iCircle(crosshairX, crosshairY, 6);
    iLine(crosshairX - 10, crosshairY, crosshairX + 10, crosshairY);
    iLine(crosshairX, crosshairY - 10, crosshairX, crosshairY + 10);
}

#endif