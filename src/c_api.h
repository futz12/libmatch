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

#ifndef LIBMATCH_C_API_H
#define LIBMATCH_C_API_H

#ifdef _WIN32
#ifdef LIBMATCH_EXPORTS
#define LIBMATCH_C_API extern "C" __declspec(dllexport)
#else
#define LIBMATCH_C_API extern "C" __declspec(dllimport)
#endif
#else
#define LIBMATCH_C_API extern "C"
#endif

#include <stdint.h>


#define COLOR_MASK        0x0000000F

#define COLOR_GRAY         0x00000000
#define COLOR_BGRA         0x00000001
#define COLOR_BGR          0x00000002
#define COLOR_BGRA_COLOR   0x00000003 // 真彩模式

struct Rect {
    int x, y, width, height;
};

struct Point {
    int x, y;
};

struct objectEx {
    Rect rect;
    float prob;
};

struct objectEx2 {
    struct {
        float x, y;
    } dots[4];

    float prob;
};

struct TextBox {
    Point boxPoint[4];
    float score;
    char *text;
    int size_charPositions;
    int *charPositions;
};

struct orb_param {
    int nfeatures = 500;
    float scaleFactor = 1.2f;
    int nlevels = 8;
    int edgeThreshold = 31;
    int firstLevel = 0;
    int WTA_K = 2;
    int scoreType = 0;
    int patchSize = 31;
    int fastThreshold = 20;
};

struct RecResult {
    char *text;
    int *charPositions;
    float prob;
};

LIBMATCH_C_API void *create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode);

LIBMATCH_C_API void release_template_matcher(void *matcher);

LIBMATCH_C_API void *template_matcher_compute(void *matcher, uint8_t *src_img_data, int src_img_size,
                                              float prob_threshold, float nms_threshold);

LIBMATCH_C_API void *template_matcher_compute_rect(void *matcher, uint8_t *src_img_data, int src_img_size,
                                                   float prob_threshold, float nms_threshold, int sx, int sy, int ex,
                                                   int ey);

LIBMATCH_C_API size_t template_matcher_result_size(void *result);

LIBMATCH_C_API void template_matcher_result_get(void *result, size_t index, void *result_obj);

LIBMATCH_C_API void release_template_matcher_result(void *result);

LIBMATCH_C_API void *orb_create_featurer(uint8_t *img_data, int img_size, void *param, int mode);

LIBMATCH_C_API void orb_release_featurer(void *featurer);

LIBMATCH_C_API void *orb_create_matcher(int mode);

LIBMATCH_C_API void orb_release_matcher(void *matcher);

LIBMATCH_C_API uint32_t orb_matcher_compute(void *matcher, void *source, void *target, float thresh, void *result);

LIBMATCH_C_API void *create_ppocr(uint8_t *det_bin, int det_bin_size, char *det_param, uint8_t *rec_bin,
                                  int rec_bin_size, char *rec_param, const char *keylist, int num_thread,
                                  bool use_vulkan);

LIBMATCH_C_API void release_ppocr(void *ppocr);

LIBMATCH_C_API void *ppocr_detect(void *ppocr, uint8_t *src_img_data, int src_img_size);

LIBMATCH_C_API size_t ppocr_result_size(void *result);

LIBMATCH_C_API void ppocr_get_textbox(void *result, size_t index, void *result_obj);

LIBMATCH_C_API void release_ppocr_textbox(void *result_obj);

LIBMATCH_C_API void release_ppocr_result(void *result);

LIBMATCH_C_API void *create_ddddrec(uint8_t *rec_bin, int rec_bin_size, char *rec_param, const char *keylist,
                                    int num_thread, bool use_vulkan);

LIBMATCH_C_API void release_ddddrec(void *ddddrec);

LIBMATCH_C_API uint32_t ddddrec_detect(void *ddddrec, uint8_t *src_img_data, int src_img_size, const char *options,
                                       void *result);

LIBMATCH_C_API void release_ddddrec_result(void *result);

LIBMATCH_C_API void unregister_vulkan();

#endif //LIBMATCH_C_API_H
