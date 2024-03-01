//
// Created by Ice on 2024/3/1.
//

#include <opencv2/opencv.hpp>
#include "base_algorithm.h"


namespace libmatch {
    inline float intersection_area(const objectEx& a, const objectEx& b)
    {
        cv::Rect_<float> inter = a.rect & b.rect;
        return inter.area();
    }
    void nms_sorted_bboxes(const std::vector<objectEx>& objects, std::vector<int>& picked, float nms_threshold)
    {
        picked.clear();

        const size_t n = objects.size();

        std::vector<float> areas(n);
        for (int i = 0; i < n; i++)
        {
            areas[i] = objects[i].rect.area();
        }

        for (int i = 0; i < n; i++)
        {
            const objectEx& a = objects[i];

            int keep = 1;
            for (int j : picked)
            {
                const objectEx& b = objects[j];
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

} // libmatch