//
// Created by Ice on 2024/3/1.
//

#include <opencv2/opencv.hpp>
#include "base_algorithm.h"


namespace libmatch {
    inline float intersection_area(const objectEx &a, const objectEx &b) {
        cv::Rect_<float> inter = a.rect & b.rect;
        return inter.area();
    }

    void nms_sorted_bboxes(const std::vector<objectEx> &objects, std::vector<int> &picked, float nms_threshold) {
        picked.clear();

        const size_t n = objects.size();

        std::vector<float> areas(n);
        for (int i = 0; i < n; i++) {
            areas[i] = objects[i].rect.area();
        }

        for (int i = 0; i < n; i++) {
            const objectEx &a = objects[i];

            int keep = 1;
            for (int j: picked) {
                const objectEx &b = objects[j];
                // intersection over union
                float inter_area = intersection_area(a, b);
                float union_area = areas[i] + areas[j] - inter_area;
                // float IoU = inter_area / union_area
                if (inter_area / union_area > nms_threshold)
                    keep = 0;
            }

            if (keep)
                picked.push_back(i);

        }
    }

    // 对OpenCV的findHomography的重写，利用SVD求解
    cv::Mat Findhomography(std::vector<cv::Point2f> src, std::vector<cv::Point2f> target) {
        // 求解系统为 A*X - B = 0

        const int n = src.size();
        float x[9] = {0};

        cv::Mat A = cv::Mat::zeros(2 * n, 8, CV_32F);
        cv::Mat B = cv::Mat::zeros(2 * n, 1, CV_32F);
        cv::Mat X(8, 1, CV_32F, x);

        for (int i = 0; i < n; i++) {
            A.at<float>(i, 0) = src[i].x;
            A.at<float>(i + n, 3) = src[i].x;
            A.at<float>(i, 1) = src[i].y;
            A.at<float>(i + n, 4) = src[i].y;

            A.at<float>(i, 2) = 1;
            A.at<float>(i + n, 5) = 1;

            A.at<float>(i, 6) = -src[i].x * target[i].x;
            A.at<float>(i, 7) = -src[i].y * target[i].x;

            A.at<float>(i + n, 6) = -src[i].x * target[i].y;
            A.at<float>(i + n, 7) = -src[i].y * target[i].y;

            B.at<float>(i, 0) = target[i].x;
            B.at<float>(i + n, 0) = target[i].y;
        }

        cv::solve(A, B, X, cv::DECOMP_SVD);


        x[8] = 1;
        cv::Mat H = cv::Mat(3, 3, CV_32F, x).clone();
        return H;
    }
} // libmatch