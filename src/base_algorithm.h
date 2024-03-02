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
