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
#include "base_algorithm.h"


namespace libmatch {
    void multi_ransac(const std::vector<cv::DMatch> &matches,
                      const std::vector<cv::KeyPoint> &kp1,
                      const std::vector<cv::KeyPoint> &kp2,
                      std::vector<cv::Mat> &homographies,
                      std::vector<std::vector<uchar> > &masks,
                      double ransac_threshold,
                      int min_inliers,
                      int max_models) {
        std::vector<int> remaining_indices(matches.size());
        for (int i = 0; i < matches.size(); ++i)
            remaining_indices[i] = i;

        homographies.clear();
        masks.clear();

        for (int model_count = 0; model_count < max_models; ++model_count) {
            if (remaining_indices.size() < 4)
                break;

            // 提取当前剩余匹配的坐标
            std::vector<cv::Point2f> src_pts, dst_pts;
            for (int idx: remaining_indices) {
                src_pts.push_back(kp1[matches[idx].queryIdx].pt);
                dst_pts.push_back(kp2[matches[idx].trainIdx].pt);
            }

            // 运行RANSAC
            cv::Mat H, mask;
            H = findHomography(src_pts, dst_pts, cv::RANSAC, ransac_threshold, mask);

            // 检查内点数量
            int inlier_count = countNonZero(mask);
            if (inlier_count < min_inliers)
                break;

            // 生成全局掩码（相对于原始matches）
            std::vector<uchar> model_mask(matches.size(), 0);
            for (int j = 0; j < mask.rows; ++j) {
                if (mask.at<uchar>(j))
                    model_mask[remaining_indices[j]] = 1;
            }

            // 保存结果
            homographies.push_back(H);
            masks.push_back(model_mask);

            // 移除当前模型的内点，保留外点进行下一轮
            std::vector<int> new_remaining_indices;
            for (int j = 0; j < mask.rows; ++j) {
                if (!mask.at<uchar>(j))
                    new_remaining_indices.push_back(remaining_indices[j]);
            }
            remaining_indices.swap(new_remaining_indices);
        }
    }
} // libmatch