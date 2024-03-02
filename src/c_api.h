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

struct Rect {
    int x, y, width, height;
};
struct RectF {
    float x, y, width, height;
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
};
struct TextBox {
    Point boxPoint[4];
    float score;
    char *text;
    int size_charPositions;
    int *charPositions;
};
struct object
{
    RectF rect;
    int label;
    float prob;
};

LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode);

LIBMATCH_C_API void release_template_matcher(void* matcher);

LIBMATCH_C_API void* template_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold);

LIBMATCH_C_API void* template_matcher_compute_rect(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold, int sx, int sy, int ex, int ey);

LIBMATCH_C_API size_t template_matcher_result_size(void* result);

LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index, void* result_obj);

LIBMATCH_C_API void release_template_matcher_result(void* result);

LIBMATCH_C_API void* create_orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode);

LIBMATCH_C_API void release_orb_matcher(void* matcher);

LIBMATCH_C_API bool orb_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, void* result);

LIBMATCH_C_API void* create_ppocr(uint8_t *det_bin, int det_bin_size, char *det_param, uint8_t *rec_bin, int rec_bin_size, char *rec_param, const char *keylist, int num_thread, bool use_vulkan);

LIBMATCH_C_API void release_ppocr(void* ppocr);

LIBMATCH_C_API void* ppocr_detect(void* ppocr, uint8_t *src_img_data, int src_img_size);

LIBMATCH_C_API size_t ppocr_result_size(void* result);

LIBMATCH_C_API void ppocr_get_textbox(void* result, size_t index, void* result_obj);

LIBMATCH_C_API void release_ppocr_textbox(void* result_obj);

LIBMATCH_C_API void release_ppocr_result(void* result);

LIBMATCH_C_API void* create_yolo57(uint8_t *bin, int bin_size, char *param, char *config);

LIBMATCH_C_API void release_yolo57(void* yolo);

LIBMATCH_C_API void* yolo57_detect(void* yolo57, uint8_t *src_img_data, int src_img_size,float prob_threshold, float nms_threshold,bool agnostic);

LIBMATCH_C_API size_t yolo57_result_size(void* result);

LIBMATCH_C_API void yolo57_get_object(void* result, size_t index, void* result_obj);

LIBMATCH_C_API void release_yolo57_result(void* result);

LIBMATCH_C_API void unregister_vulkan();

LIBMATCH_C_API void* create_window_capture(void *hwnd, int sx, int sy, int ex, int ey);

LIBMATCH_C_API void release_window_capture(void *capture);

LIBMATCH_C_API void capture_update(void *capture);

LIBMATCH_C_API int capture_get_bitmap(void *capture,void **bitmap);

#endif //LIBMATCH_C_API_H
