// Minimal image loader shim using GDI+ so the project can load PNG/JPEG/PNG
// into RGBA buffers. This provides the two functions used by the project:
//    unsigned char* stbi_load(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels);
//    void stbi_image_free(void* retval_from_stbi_load);
//
// The real project originally used the public-domain stb_image.h. To avoid
// shipping the whole single-file here, this small compatible wrapper uses
// GDI+ on Windows to load files and returns a 4-channel (RGBA) buffer.

#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <stdlib.h>
#include <string>

#pragma comment(lib, "gdiplus")

typedef unsigned char stbi_uc;

static void stbi__gdip_init()
{
    static bool inited = false;
    if (!inited) {
        Gdiplus::GdiplusStartupInput gdipsi;
        static ULONG_PTR gdipToken = 0;
        Gdiplus::GdiplusStartup(&gdipToken, &gdipsi, NULL);
        inited = true;
    }
}

static std::wstring stbi__utf8_to_wide(const char* utf8)
{
    if (!utf8) return std::wstring();
    int sz = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (sz <= 0) return std::wstring();
    std::wstring w; w.resize(sz - 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &w[0], sz);
    return w;
}

#ifdef STB_IMAGE_IMPLEMENTATION

unsigned char* stbi_load(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels)
{
    stbi__gdip_init();
    if (!filename) return NULL;
    std::wstring wpath = stbi__utf8_to_wide(filename);
    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(wpath.c_str());
    if (!bmp) return NULL;
    if (bmp->GetLastStatus() != Gdiplus::Ok) { delete bmp; return NULL; }

    int width = bmp->GetWidth();
    int height = bmp->GetHeight();
    Gdiplus::PixelFormat fmt = PixelFormat32bppARGB;

    Gdiplus::Rect rect(0, 0, width, height);
    Gdiplus::BitmapData bd;
    if (bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, fmt, &bd) != Gdiplus::Ok) { delete bmp; return NULL; }

    int out_channels = 4;
    int stride = width * out_channels;
    unsigned char* out = (unsigned char*)malloc(width * height * out_channels);
    if (!out) { bmp->UnlockBits(&bd); delete bmp; return NULL; }

    for (int row = 0; row < height; ++row) {
        unsigned char* src = (unsigned char*)bd.Scan0 + row * bd.Stride;
        unsigned char* dst = out + row * stride;
        for (int col = 0; col < width; ++col) {
            // GDI+ returns BGRA (byte order may vary); convert to RGBA
            unsigned char b = src[col * 4 + 0];
            unsigned char g = src[col * 4 + 1];
            unsigned char r = src[col * 4 + 2];
            unsigned char a = src[col * 4 + 3];
            dst[col * 4 + 0] = r;
            dst[col * 4 + 1] = g;
            dst[col * 4 + 2] = b;
            dst[col * 4 + 3] = a;
        }
    }

    bmp->UnlockBits(&bd);
    delete bmp;

    if (x) *x = width;
    if (y) *y = height;
    if (channels_in_file) *channels_in_file = out_channels;
    (void)desired_channels; // we always return 4 channels
    return out;
}

void stbi_image_free(void* retval_from_stbi_load)
{
    if (retval_from_stbi_load) free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

