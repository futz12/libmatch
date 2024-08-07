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

#ifndef LIBMATCH_BASE_MATCH_H
#define LIBMATCH_BASE_MATCH_H


namespace libmatch {
#define COLOR_MASK        0x0000000F

#define COLOR_GRAY         0x00000000
#define COLOR_BGRA         0x00000001
#define COLOR_BGR          0x00000002
#define COLOR_BGRA_COLOR   0x00000003 // 真彩模式
#define COLOR_BGR_MASK     0x00000004 // bgr 模式
#define COLOR_GRAY_MASK    0x00000005 // gray 模式

#define ORB_PRE_MASK      0x0000000F // orb 预处理模式
#define ORB_PRE_NONE      0x00000000
#define ORB_PRE_CANN      0x00000001 // 使用canny边缘检测

#define READ_OVER() if (target_mat.empty()) {fprintf(stderr,"[Match] Err Can`t Read Image");return ;}
#define READ_OVER_SRC() if (src_mat.empty()) {fprintf(stderr,"[Match] Err Can`t Read Image");return {};}

    class template_matcher {
    private:
        cv::Mat target_mat;
        cv::Mat mask_mat;
        uint32_t _mode;

    public:
        template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode);

        std::vector<objectEx>
        compute(uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold,
                int sx = 0, int sy = 0, int ex = -1, int ey = -1);
    }; // 模板匹配匹配子

    struct orb_param {
        int nfeatures = 500;
        float scaleFactor = 1.2f;
        int nlevels = 8;
        int edgeThreshold = 31;
        int firstLevel = 0;
        int WTA_K = 2;
        int scoreType = 0;
        int patchSize = 31;
        int fastThreshold = 20;
    };

    // orb 特征子
    class orb_featurer {
    private:
        std::vector<cv::KeyPoint> m_kps;
        cv::Mat m_desc;
        cv::Mat m_desc_fp32;
        cv::Size m_size;
        friend class orb_matcher;

    public:
        orb_featurer(const uint8_t *img_data, int img_size, orb_param param, uint32_t mode);
    };

    enum match_mode {
        flann = 0,
        hamming = 1,
    };

    //orb 匹配器
    class orb_matcher {
    private:
        uint32_t m_mode{};
        cv::Ptr<cv::DescriptorMatcher> matcher;

        uint32_t flann_matcher(orb_featurer &source, orb_featurer &target, float thresh,
                               objectEx2 *res);

        uint32_t hamming_matcher(orb_featurer &source, orb_featurer &target, float thresh,
                                 objectEx2 *res);

    public:
        explicit orb_matcher(uint32_t mode);

        uint32_t match(orb_featurer &source,orb_featurer &target, float thresh,
                       objectEx2 *res);
    };
} // libmatch

#endif //LIBMATCH_BASE_MATCH_H
