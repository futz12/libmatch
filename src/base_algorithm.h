//
// Created by Ice on 2024/3/1.
//

#ifndef LIBMATCH_BASE_ALGORITHM_H
#define LIBMATCH_BASE_ALGORITHM_H


#include <vector>
#include <cstdint>

namespace libmatch {
    struct objectEx {
        cv::Rect rect;
        float prob;
    };
    struct objectEx2 {
        struct {
            float x, y;
        } dots[4];
    };
    struct object
    {
        cv::Rect_<float> rect;
        int label;
        float prob;
    };

#ifdef LIBMATCH_EXPORTS
    inline float intersection_area(const objectEx &a, const objectEx &b);
    void nms_sorted_bboxes(const std::vector<objectEx> &objects, std::vector<int> &picked, float nms_threshold);
    void nms_sorted_bboxes(const std::vector<object> &objects, std::vector<int>& picked, float nms_threshold, bool agnostic = false);
    cv::Mat Findhomography(std::vector<cv::Point2f> src, std::vector<cv::Point2f> target);
    inline float sigmoid(float x)
    {
        return static_cast<float>(1.f / (1.f + exp(-x)));
    }
#endif
} // libmatch

#endif //LIBMATCH_BASE_ALGORITHM_H
