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

        void *data = nullptr;

        int _sx, _sy;

        BITMAPINFO bi = {0};

    public:
        uint8_t *bitmap;
        int bitmap_size = 0;

        window_capture(HWND hwndTarget, int sx, int sy, int ex, int ey);

        ~window_capture();

        void update();
    };
};
#endif

#endif //LIBMATCH_SCREENSHOT_H
