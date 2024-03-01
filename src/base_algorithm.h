//
// Created by Ice on 2024/3/1.
//

#ifndef LIBMATCH_BASE_ALGORITHM_H
#define LIBMATCH_BASE_ALGORITHM_H

#ifdef LIBMATCH_EXPORTS
#define LIBMATCH_API __declspec(dllexport)
#else
#define LIBMATCH_API extern __declspec(dllimport)
namespace cv
{
    struct Rect {
        float x, y, width, height;
    };
}
#endif

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

#ifdef LIBMATCH_EXPORTS
    inline float intersection_area(const objectEx &a, const objectEx &b);
    void nms_sorted_bboxes(const std::vector<objectEx> &objects, std::vector<int> &picked, float nms_threshold);
#endif
} // libmatch

#endif //LIBMATCH_BASE_ALGORITHM_H
