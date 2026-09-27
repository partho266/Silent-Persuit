# Silent Pursuit — Checkpoint update

`iMain.cpp` went from **1902 lines in one file** to **262 lines plus 11 headers**.
Nothing about how the game plays was changed by the restructuring — the only
behaviour changes are the four Level 1 features that were requested.

---

## 1. Files

Every new file sits next to `iMain.cpp`, inside
`SilentPersuit/iGraphics files (with Updated Keyboard Functions)/`.
They are already added to `SilentPersuit.vcxproj`, so they appear under
**Header Files** in Solution Explorer when you open the solution.

They must be included in this order, because each one uses the ones above it:

| # | File | What is inside |
|---|------|----------------|
| 1 | `Config.h` | Every number in the game: window size, gravity, ground heights, object positions, colours. Also the `GameState` enum and small helpers (`countDown`, `clampD`, `distance2D`). |
| 2 | `Draw.h` | The only file in the project that contains OpenGL. Wraps the two things iGraphics cannot do: see-through colours and mirrored images. |
| 3 | `Assets.h` | Every texture handle, every sprite size, and `loadAllAssets()`. |
| 4 | `World.h` | Camera, screen shake, `groundYAt()`, and the drawing of the platform, rock and block. |
| 5 | `Effects.h` | Muzzle flash, bullet hit marker, ground shadow, explosion, screen flash. |
| 6 | `UI.h` | `Button` struct, all buttons, `drawButton`, mouse-to-game coordinates, crosshair. |
| 7 | `Player.h` | The assassin: state, facing rule, animation frames, drawing. |
| 8 | `Enemy.h` | The two guards, their hit areas, their AI and drawing. |
| 9 | `Landmine.h` | The mine: arming, triggering, blast damage, drawing. |
| 10 | `Level1.h` | The rules of Level 1: input, collision, gravity, firing. |
| 11 | `Screens.h` | One draw function per screen. |
| — | `iMain.cpp` | Only the wiring: `iDraw`, the mouse and keyboard callbacks, `fixedUpdate`, `main`. |

## 2. Readability changes (no functionality changed)

* **All OpenGL is now in `Draw.h`.** The rest of the game only calls lab
  functions — `iFilledCircle`, `iFilledEllipse`, `iFilledRectangle`,
  `iFilledPolygon`, `iLine`, `iText`, `iShowImage`. This works because those
  iGraphics functions do not pick a colour themselves, they draw with whatever
  colour is currently set, so `setColorAlpha()` can be used right before them.
* **`0.02` is now `TICK`.** Every countdown in the game was subtracting a bare
  `0.02`. There is now one named constant and one helper, `countDown(timer)`.
* **`fixedUpdate` was 260 lines.** It is now `updateLevel1()` in `Level1.h`,
  split into named steps: `applyHorizontalMovement`, `applyLevelBoundaries`,
  `applyGravityAndLanding`, `tryJump`, `wakeEnemiesInView`, `updateFiring`.
* **`iDraw` was 400 lines of `if/else`.** It is now seven one-line calls.
* **`&e == &enemy1` is gone.** The enemy struct has a `fallsOffLedge` flag
  instead, so the death behaviour is a property of the guard rather than a
  pointer comparison.
* **The two guards are set up by one function**, `resetEnemy()`, instead of
  two blocks of 14 near-identical assignments.

## 3. Level 1 features

### (a) Facing direction
Previously the body always turned towards the mouse pointer, so the assassin
ran backwards whenever the crosshair was behind him.

There is now a priority list in `updatePlayerFacing()` (`Player.h`):

1. the mouse is being **moved** → face the crosshair
2. walking left / right → face the walking direction
3. in the air → face the direction of the jump
4. standing still → keep the last direction

"The mouse is active" means the pointer actually changed position within the
last `MOUSE_AIM_HOLD` seconds (1.2 s, in `UI.h`). Clicking to fire also counts,
so aiming and shooting still turns him even if the pointer is held still.
The crosshair is drawn bright while the mouse is in control and dim when the
keyboard has taken over, so it is visible on screen which one is steering.

### (b) Headshots
Each guard now has **two** hit areas instead of one circle:

| Area | Shape | Damage |
|------|-------|--------|
| Head | circle, radius ≈ 23 px | `ENEMY_MAX_HP / 2` = **25** — half his total HP |
| Body | box over chest, belly and legs | 10 |

So two clean headshots kill a guard, versus five body shots.

### (c) Better bullet damage areas
The old code tested **one** circle of radius 55 in the middle of the sprite,
and only ever against the *first* living guard. That meant shots at the head
or legs did nothing, shots at empty space beside the guard counted as hits,
and the second guard could not be hit while the first was alive.

Now `findShotTarget()` checks **every** guard, head first, and a hit only
registers inside the actual head circle or body box. A hit marker pops at the
bullet's landing point — yellow for a headshot, red for a body shot — and
**HEADSHOT!** flashes on the HUD.

Press **H** during play to draw the head circle and body box on screen. This
is the fastest way to re-tune the numbers at the top of `Enemy.h` if the
sprites are ever swapped out.

### (d) Landmine blast
The mine now explodes properly whether it is **stepped on** or **shot**:

* white screen flash
* expanding shockwave ring
* fireball → smoke cloud → rolling dust
* 14 pieces of debris thrown out and falling back down
* the camera shakes for 0.55 s

Stepping on it is still instant death. Shooting it now depends on where you
are standing when it goes off:

| Distance from mine | Result |
|---|---|
| under 70 px | killed |
| 70 – 190 px | wounded, scaled by distance (up to 70 HP) |
| over 190 px | safe — the mine is cleared |

The mine's warning light also blinks now, driven by `animTime`.

## 4. Tuning

Almost everything worth changing is a named constant at the top of a file:

* damage and HP → top of `Enemy.h`
* blast radii and damage → top of `Landmine.h`
* how long the mouse keeps control of facing → `MOUSE_AIM_HOLD` in `UI.h`
* head circle and body box → `HEAD_CENTER_X/Y`, `HEAD_RADIUS`, `BODY_*` in `Enemy.h`
* level layout and physics → `Config.h`

---

## 5. Build fix (added after the first build attempt)

The first version failed to compile with ~100 errors like
`function 'void stbi__start_mem(...)' already has a body` and
`'CALLBACK': macro redefinition`.

**Cause:** `iGraphics.h` has no include guard, and it *defines* real
functions and variables rather than only declaring them. Each of the new
headers had its own `#include "iGraphics.h"`, so the entire library — plus
`stb_image.h` with `STB_IMAGE_IMPLEMENTATION` and `windows.h` — was pasted
into the same .cpp file twelve times. Every function then existed twelve
times over.

**Fix, two parts:**

1. Only `iMain.cpp` includes `iGraphics.h`, once, before everything else.
   Each header now carries a short comment saying why it must not include it.
2. An include guard (`#ifndef IGRAPHICS_H`) was added to `iGraphics.h`, so a
   stray include in the future is harmless instead of fatal.

### If you also saw the `SetTimer` / `TIMERPROC` errors

```
'UINT_PTR SetTimer(HWND,UINT_PTR,UINT,TIMERPROC)': cannot convert argument 4
```

That one is **not** from our code and not from the restructuring. It appears
only when the Solution Platform is set to **x64**, because `UINT_PTR` becomes
64-bit there while `iGraphics.h` declares its timer callbacks with
`unsigned int`.

Set the platform dropdown in the Visual Studio toolbar to **x86** (Win32).
That is the only platform this project can use anyway: the supplied
`glut32.lib`, `Glaux.lib` and `GLU32.LIB` contain 32-bit code only, so an
x64 build would fail at the linker even if it compiled.
