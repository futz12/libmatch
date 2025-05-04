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
#include <opencv2/opencv.hpp>

namespace libmatch {
    struct objectEx {
        cv::Rect rect;
        float prob;
    };

    struct objectEx2 {
        struct {
            float x, y;
        } dots[4];

        float prob;
    };

    /*
    * 多目标RANSAC拟合多个单应性矩阵
    * 参数:
    *    matches: 初步筛选后的匹配点列表 (DMatch对象)
    *    kp1, kp2: 两幅图像的SIFT关键点
    *    homographies: 输出的单应性矩阵列表
    *    masks: 输出的每个模型对应的内点掩码列表（每个掩码与原始matches长度相同）
    *    ransac_threshold: RANSAC内点阈值（像素，默认5.0）
    *    min_inliers: 模型有效的最小内点数（默认50）
    *    max_models: 最多检测的模型数量（默认5）
    */
    void multi_ransac(const std::vector<cv::DMatch> &matches,
                      const std::vector<cv::KeyPoint> &kp1,
                      const std::vector<cv::KeyPoint> &kp2,
                      std::vector<cv::Mat> &homographies,
                      std::vector<std::vector<uchar> > &masks,
                      double ransac_threshold = 5.0,
                      int min_inliers = 50,
                      int max_models = 5);
} // libmatch

#endif //LIBMATCH_BASE_ALGORITHM_H
