//
// Created by Ice on 2024/3/2.
//

#ifndef LIBMATCH_SCREENSHOT_H
#define LIBMATCH_SCREENSHOT_H

#ifdef _WIN32

#include <Windows.h>
#include <wingdi.h>

namespace libmatch {
    class window_capture {
    private:
        HWND hwndTarget;
        HDC hdcTarget;
        HDC memDC;
        HBITMAP oldBitmap;
        HBITMAP hBitmap;

        void *data;

        int _sx, _sy;

        BITMAPINFO bi = {0};

    public:
        uint8_t *bitmap;
        int bitmap_size = 0;

        window_capture(HWND hwndTarget, int sx, int sy, int ex, int ey) {
            if (!IsWindow(hwndTarget)) {
                fprintf(stderr, "Invalid hwndTarget");
                return;
            }
            this->hwndTarget = hwndTarget;
            hdcTarget = GetWindowDC(hwndTarget);
            memDC = CreateCompatibleDC(hdcTarget);
            RECT rect;
            GetWindowRect(hwndTarget, &rect);

            float scale = (float)GetDpiForWindow(hwndTarget) / USER_DEFAULT_SCREEN_DPI;

            rect.right = (int)(rect.right * scale);
            rect.bottom = (int)(rect.bottom * scale);
            rect.left = (int)(rect.left * scale);
            rect.top = (int)(rect.top * scale);

            if (ex == -1) ex = rect.right - rect.left;
            if (ey == -1) ey = rect.bottom - rect.top;

            // 24位色
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = ex - sx + 1;
            bi.bmiHeader.biHeight = ey - sy + 1;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 24;
            bi.bmiHeader.biCompression = BI_RGB;

            int szbits = (bi.bmiHeader.biWidth * 3 + (4 - (bi.bmiHeader.biWidth * 3) % 4) % 4) * bi.bmiHeader.biHeight;
            bi.bmiHeader.biSizeImage = szbits;

            BITMAPFILEHEADER bfh;
            memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
            bfh.bfType = 0x4D42; // 'BM'
            bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + szbits;
            bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

            bitmap_size = bfh.bfOffBits + szbits;

            bitmap = new uint8_t[bitmap_size];
            memcpy(bitmap, &bfh, sizeof(BITMAPFILEHEADER));
            memcpy(bitmap + sizeof(BITMAPFILEHEADER), &bi, sizeof(BITMAPINFOHEADER));

            hBitmap = CreateDIBSection(hdcTarget, &bi, DIB_RGB_COLORS, &data, NULL, 0);
            oldBitmap = (HBITMAP) SelectObject(memDC, hBitmap);

            _sx = sx - 1, _sy = sy - 1;
        }

        ~window_capture() {
            SelectObject(memDC, oldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(memDC);
            ReleaseDC(hwndTarget, hdcTarget);
            delete[] bitmap;
        }

        void update() {
            BitBlt(memDC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, hdcTarget, _sx, _sy, SRCCOPY);
            memcpy(bitmap + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), data, bi.bmiHeader.biSizeImage);
        }
    };
};
#endif

#endif //LIBMATCH_SCREENSHOT_H
