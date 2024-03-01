//
// Created by Ice on 2024/3/1.
//
#include <opencv2/opencv.hpp>
#include <cstring>

#include "base_algorithm.h"
#include "base_match.h"

#include "c_api.h"


LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode)
{
    return new libmatch::template_matcher(target_img_data, target_img_size, mode);
}

LIBMATCH_C_API void release_template_matcher(void* matcher)
{
    delete (libmatch::template_matcher*)matcher;
}

LIBMATCH_C_API void* template_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold)
{
    return new std::vector<libmatch::objectEx>(((libmatch::template_matcher*)matcher)->compute(src_img_data, src_img_size, prob_threshold, nms_threshold));
}

LIBMATCH_C_API size_t template_matcher_result_size(void* result)
{
    return ((std::vector<libmatch::objectEx>*)result)->size();
}

LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index,void* result_obj)
{
    auto& obj = ((std::vector<libmatch::objectEx>*)result)->at(index);
    memcpy(result_obj, &obj, sizeof(libmatch::objectEx));
}

LIBMATCH_C_API void release_template_matcher_result(void* result)
{
    delete (std::vector<libmatch::objectEx>*)result;
}

LIBMATCH_C_API void* create_orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode)
{
    return new libmatch::orb_matcher(target_img_data, target_img_size, n_features, mode);
}

LIBMATCH_C_API void release_orb_matcher(void* matcher)
{
    delete (libmatch::orb_matcher*)matcher;
}

LIBMATCH_C_API bool orb_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, void* result)
{
    return ((libmatch::orb_matcher*)matcher)->compute(src_img_data, src_img_size, n_features, max_distance, (libmatch::objectEx2*)result);
}