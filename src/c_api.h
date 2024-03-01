//
// Created by Ice on 2024/3/1.
//

#ifndef LIBMATCH_C_API_H
#define LIBMATCH_C_API_H

#ifdef LIBMATCH_EXPORTS
#define LIBMATCH_C_API extern "C" __declspec(dllexport)
#else
#define LIBMATCH_C_API extern "C" __declspec(dllimport)
struct Rect {
    float x, y, width, height;
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
#endif

#include <stdint.h>

LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode);

LIBMATCH_C_API void release_template_matcher(void* matcher);

LIBMATCH_C_API void* template_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold);

LIBMATCH_C_API size_t template_matcher_result_size(void* result);

LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index, void* result_obj);

LIBMATCH_C_API void release_template_matcher_result(void* result);

LIBMATCH_C_API void* create_orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode);

LIBMATCH_C_API void release_orb_matcher(void* matcher);

LIBMATCH_C_API bool orb_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, void* result);



#endif //LIBMATCH_C_API_H
