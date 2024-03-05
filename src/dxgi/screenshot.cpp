/*
The MIT License (MIT)
Copyright © 2024 <libmatch>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the “Software”), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include <cstdint>
#include <cstdio>
#include "screenshot.h"

// 判断是否是为Windows
#ifdef _WIN32

namespace libmatch {
    window_capture::window_capture(HWND hwndTarget, int sx, int sy, int ex, int ey) {
        if (!IsWindow(hwndTarget)) {
            fprintf(stderr, "Invalid hwndTarget");
            return;
        }
        this->hwndTarget = hwndTarget;
        hdcTarget = GetWindowDC(hwndTarget);
        memDC = CreateCompatibleDC(hdcTarget);
        RECT rect;
        GetWindowRect(hwndTarget, &rect);

        float scale = (float) GetDeviceCaps(hdcTarget, LOGPIXELSX) / USER_DEFAULT_SCREEN_DPI;

        rect.right = (int) (rect.right * scale);
        rect.bottom = (int) (rect.bottom * scale);
        rect.left = (int) (rect.left * scale);
        rect.top = (int) (rect.top * scale);

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

    window_capture::~window_capture() {
        SelectObject(memDC, oldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        ReleaseDC(hwndTarget, hdcTarget);
        delete[] bitmap;
    }

    void window_capture::update() {
        BitBlt(memDC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, hdcTarget, _sx, _sy, SRCCOPY);
        memcpy(bitmap + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), data, bi.bmiHeader.biSizeImage);
    }
}
#endif
