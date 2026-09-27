# How to build Silent Pursuit

## Step 1 — start from a clean folder

1. Close Visual Studio.
2. **Delete your whole old `SilentPersuit-main` folder**, including the
   hidden `.vs` folder inside it. That hidden folder is where Visual Studio
   remembers "last time you built x64", and it is why the platform kept
   coming back.
3. Extract this zip fresh, somewhere simple like `C:\SilentPursuit`.
4. Open `SilentPersuit.sln`.

## Step 2 — build

**Build → Rebuild Solution**, then press **F5**.

There is nothing to configure. The platform dropdown now only offers **x86**,
because the x64 configuration has been deleted from the project. It is no
longer possible to pick the setting that was failing.

---

# What was fixed in this round

## 1. x64 has been removed from the project

Previously the solution offered both x86 and x64, and the build kept landing
on x64 — which cannot work, because the bundled `glut32.lib`, `Glaux.lib`,
`GLU32.LIB` and `glui32.lib` contain 32-bit code only.

Rather than relying on the dropdown being set correctly, the x64
configuration is now gone:

* `SilentPersuit.sln` — 6 x64 lines removed
* `SilentPersuit.vcxproj` — 10 x64 elements removed

The dropdown now lists **x86 only**. Both files were checked afterwards and
are still valid.

## 2. The hardcoded library path is gone

This is a second reason the project would not have built on your machine even
after switching to x86. The project looked for the OpenGL libraries at:

```
C:\Users\daspa\source\repos\SilentPersuit\SilentPersuit\iGraphics files (...)
```

That is Partho's folder on Partho's PC. Anywhere else, the linker would fail
with `LNK1104: cannot open file 'glut32.lib'`.

All four occurrences now use a relative path instead:

```
$(ProjectDir)iGraphics files (with Updated Keyboard Functions)
```

`$(ProjectDir)` is filled in by Visual Studio at build time, so the project
now works from any folder, on any of your three machines.

## 3. The platform message

The `#error` you saw was a guard added last round, doing its job — it was
telling you the build was still x64. Since x64 is no longer selectable it
should never appear again. It is still there as a backstop, and now tells you
to delete the `.vs` folder if it ever does fire.

---

# Fixed in earlier rounds (still in place)

| Problem | Cause | Fix |
|---|---|---|
| ~60 × `stbi__... already has a body` | `stb_image.h`'s implementation block sits outside its own include guard | second guard added around the implementation block |
| `'CALLBACK': macro redefinition` | `iGraphics.h` included `glut.h` before `windows.h`; both define `CALLBACK` | `windows.h` moved first |
| 12 × `SetTimer ... cannot convert to 'TIMERPROC'` | callbacks declared `unsigned int` where Windows expects `UINT_PTR` — same type on 32-bit, different on 64-bit | all 11 callbacks retyped to `void CALLBACK (HWND, UINT, UINT_PTR, DWORD)` |
| library pasted in 12 times | `iGraphics.h` has no include guard and every new header included it | only `iMain.cpp` includes it, and a guard was added |
