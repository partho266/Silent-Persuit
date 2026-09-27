#ifndef DRAW_H
#define DRAW_H

// =========================================================
// SILENT PURSUIT - Draw.h
// ---------------------------------------------------------
// The lab iGraphics library already gives us almost every
// shape we need: iLine, iRectangle, iFilledRectangle,
// iCircle, iFilledCircle, iEllipse, iFilledEllipse,
// iPolygon, iFilledPolygon, iText and iShowImage.
//
// Only two things are missing:
//      1. see-through (transparent) colours
//      2. mirrored / flipped images
//
// Those two need a few OpenGL calls. Instead of spreading
// OpenGL all over the game, EVERY OpenGL call in the whole
// project is collected in this one small file and wrapped in
// a simple function. The rest of the code then keeps using
// normal iGraphics functions.
//
// The trick that makes this work: iFilledCircle,
// iFilledEllipse, iFilledRectangle, iFilledPolygon and iLine
// do NOT choose a colour themselves - they simply draw with
// whatever colour is currently set. So we can set a
// see-through colour first and then call them as usual.
// =========================================================

// NOTE: "iGraphics.h" is NOT included here.
// The lab library has no include guard, so including it more
// than once in the same .cpp file would paste the whole library
// (and stb_image) in twice and cause hundreds of "already has a
// body" errors. iMain.cpp includes it once, before these headers.
#include "Config.h"


// ---------------------------------------------------------
// 1. TRANSPARENCY
// ---------------------------------------------------------
// Always used as a group:
//
//      beginTransparency();
//      setColorAlpha(0, 0, 0, 0.5);      // half see-through black
//      iFilledCircle(x, y, 30);          // normal iGraphics call
//      endTransparency();
// ---------------------------------------------------------

void beginTransparency()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void endTransparency()
{
    glDisable(GL_BLEND);
}

// Works like iSetColor (r, g, b are 0-255), plus an alpha
// value from 0.0 (invisible) to 1.0 (solid).
void setColorAlpha(int r, int g, int b, double alpha)
{
    glColor4f((float)(r / 255.0), (float)(g / 255.0), (float)(b / 255.0), (float)alpha);
}


// ---------------------------------------------------------
// 2. LINE THICKNESS
// ---------------------------------------------------------
// iLine always draws a one-pixel line. This makes the next
// lines thicker. Remember to set it back to 1.
// ---------------------------------------------------------

void setLineThickness(double pixels)
{
    glLineWidth((float)pixels);
}


// ---------------------------------------------------------
// 3. MIRRORED IMAGES
// ---------------------------------------------------------
// Each character only has artwork for ONE direction (the
// assassin faces right, the guards face left). To show the
// other direction we mirror the picture around a vertical
// line running through the character (mirrorLineX), instead
// of loading a second set of images.
// ---------------------------------------------------------

void drawImageFlipped(double x, double y, int w, int h, unsigned int texture,
                      bool flip, double mirrorLineX)
{
    if (texture == 0) return;

    beginTransparency();          // so the PNG background stays invisible

    glPushMatrix();
    glTranslated(mirrorLineX, 0.0, 0.0);
    if (flip) glScaled(-1.0, 1.0, 1.0);
    glTranslated(-mirrorLineX, 0.0, 0.0);

    iShowImage((int)x, (int)y, w, h, texture);

    glPopMatrix();
    endTransparency();
}

// When a picture is mirrored, a point that used to sit at X
// ends up on the other side of the mirror line. This does
// that flip for one X value. We need it for the muzzle flash
// position and for the bullet hit tests.
double mirrorX(double x, double mirrorLineX, bool flip)
{
    if (!flip) return x;
    return 2.0 * mirrorLineX - x;
}

#endif
