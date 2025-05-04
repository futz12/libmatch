#ifndef LIBMATCH_BASE_MATCH_H
#define LIBMATCH_BASE_MATCH_H

#include <opencv2/core.hpp>
#include <vector>
#include <cstdio>

namespace libmatch {
    // 保留原始宏定义
#define COLOR_MASK          0x0000000F
#define COLOR_GRAY          0x00000000
#define COLOR_BGRA          0x00000001
#define COLOR_BGR           0x00000002
#define COLOR_BGRA_COLOR    0x00000003
#define COLOR_BGR_MASK      0x00000004
#define COLOR_GRAY_MASK     0x00000005

#define MODEL_MASK          0x000000F0
#define MODEL_CCOEFF_NORMED 0x00000000
#define MODEL_SQDIFF_NORMED 0x00000010

    class template_matcher {
    private:
        cv::Mat target_mat;
        cv::Mat mask_mat;
        uint32_t _mode;

        // 统一错误处理函数
        template<typename T>
        inline bool validate_image(const cv::Mat &img, const char *context, T ret_val) const {
            if (!img.empty()) return true;

            fprintf(stderr, "[Match] Error: %s\n"
                    "  File: %s\n"
                    "  Line: %d\n"
                    "  Mode: 0x%08X\n",
                    context, __FILE__, __LINE__, _mode);
            return false;
        }

        // 处理流程分解
        void decode_target(uint8_t *data, int size);

        void process_alpha_channel();

        void generate_color_mask();

        void setup_roi(cv::Mat &src, cv::Rect &roi) const;

        void match_template(const cv::Mat &src, cv::Mat &result) const;

    public:
        template_matcher(uint8_t *target_img_data,
                         int target_img_size,
                         uint32_t mode);

        std::vector<objectEx> compute(const uint8_t *src_img_data,
                                      int src_img_size,
                                      float prob_threshold = 0.6f,
                                      float nms_threshold = 0.4f,
                                      cv::Rect roi = cv::Rect());
    };

#define SIFT_MODE  0x00000000
#define ORB_MODE   0x00000001
#define AKAZE_MODE 0x00000002


    struct feature_config {
        int detector_type = SIFT_MODE;

        union {
            struct {
                int nfeatures = 0;
                int nOctaveLayers = 3;
                double contrastThreshold = 0.04;
                double edgeThreshold = 10;
                double sigma = 1.6;
            } sift;

            struct {
                int nfeatures = 500;
                float scaleFactor = 1.2f;
                int nlevels = 8;
                int edgeThreshold = 31;
                int firstLevel = 0;
                int WTA_K = 2;
                int scoreType = cv::ORB::HARRIS_SCORE;
                int patchSize = 31;
                int fastThreshold = 20;
            } orb;

            struct {
                int descriptor_type = cv::AKAZE::DESCRIPTOR_MLDB;
                int descriptor_size = 0;
                int descriptor_channels = 3;
                int threshold = 0.001f;
                int nOctaves = 4;
                int nOctaveLayers = 4;
                int diffusivity = cv::KAZE::DIFF_PM_G2;
                int max_points = -1;
            } akaze;
        } params;
    };

    class feature_detector {
    private:
        std::vector<cv::KeyPoint> keypoints_;
        cv::Mat descriptors_;
        cv::Size input_size_;

        // 图像解码方法
        static cv::Mat decode_image(uint8_t *data, int size) {
            cv::_InputArray arr(data, size);
            return cv::imdecode(arr, cv::IMREAD_GRAYSCALE);
        }

    public:
        feature_detector(uint8_t *input_img_data,
                         int input_img_size,
                         feature_config *config);

        // 获取特征检测结果
        const std::vector<cv::KeyPoint> &getKeypoints() const { return keypoints_; }
        const cv::Mat &getDescriptors() const { return descriptors_; }
        const cv::Size &getImageSize() const { return input_size_; }
    };

    // 宏定义匹配器类型
#define MATCHER_FLOAT  0
#define MATCHER_BINARY 1

    struct match_config {
        int matcher_type = MATCHER_FLOAT;
        union {
            int max_hanming_distance;
            float ratio_thresh;
        } thresh;

        double ransac_threshold = 5.0; // 重投影误差阈值
        int min_inliers = 50; // 最小内点数
        int max_models = 5; // 最大模型数
    };

    std::vector<std::vector<cv::Point2f> > feature_match(
        const feature_detector &query,
        const feature_detector &source,
        const match_config &config);
} // namespace libmatch
#endif // LIBMATCH_BASE_MATCH_H
