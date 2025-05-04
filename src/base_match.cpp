#include "base_algorithm.h"
#include "base_match.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>


namespace libmatch {
    using namespace cv;

    template_matcher::template_matcher(uint8_t *target_img_data,
                                       int target_img_size,
                                       uint32_t mode) : _mode(mode) {
        decode_target(target_img_data, target_img_size);

        // 处理特殊模式
        switch (mode & COLOR_MASK) {
            case COLOR_BGRA:
            case COLOR_BGRA_COLOR:
                process_alpha_channel();
                break;
            case COLOR_BGR_MASK:
            case COLOR_GRAY_MASK:
                generate_color_mask();
                break;
        }
    }

    void template_matcher::decode_target(uint8_t *data, int size) {
        const int color_flag = [](uint32_t mode) {
            switch (mode & COLOR_MASK) {
                case COLOR_GRAY: return cv::IMREAD_GRAYSCALE;
                case COLOR_BGRA: // 延续处理
                case COLOR_BGRA_COLOR: return IMREAD_UNCHANGED;
                default: return IMREAD_COLOR;
            }
        }(_mode);

        Mat raw_data(1, size, CV_8UC1, data);
        target_mat = imdecode(raw_data, color_flag);
        if (!validate_image(target_mat, "Target image decode failed", nullptr))
            return;
    }

    void template_matcher::process_alpha_channel() {
        if (target_mat.channels() != 4) {
            fprintf(stderr, "BGRA image requires 4 channels (actual: %d)\n",
                    target_mat.channels());
            return;
        }

        // 分离alpha通道
        Mat channels[4];
        split(target_mat, channels);
        channels[3].convertTo(mask_mat, CV_32FC1, 1.0f / 255.0f);

        // 转换颜色空间
        const int conversion = (_mode & COLOR_MASK) == COLOR_BGRA ? COLOR_BGRA2GRAY : COLOR_BGRA2BGR;
        cvtColor(target_mat, target_mat, conversion);
    }

    void template_matcher::generate_color_mask() {
        const Vec3b mask_color = target_mat.at<Vec3b>(0, 0);

        // 向量化掩码生成
        Mat mask_8u;
        inRange(target_mat, Scalar(mask_color), Scalar(mask_color), mask_8u);
        bitwise_not(mask_8u, mask_8u);
        mask_8u.convertTo(mask_mat, CV_32FC1, 1.0f / 255.0f);

        // 灰度转换
        if ((_mode & COLOR_MASK) == COLOR_GRAY_MASK) {
            cvtColor(target_mat, target_mat, COLOR_BGR2GRAY);
        }
    }

    std::vector<objectEx> template_matcher::compute(const uint8_t *src_img_data,
                                                    int src_img_size,
                                                    float prob_threshold,
                                                    float nms_threshold,
                                                    cv::Rect roi) {
        // 解码源图像
        Mat src_mat = imdecode(Mat(1, src_img_size, CV_8UC1,
                                   const_cast<uint8_t *>(src_img_data)),
                               [this]() {
                                   return ((_mode & COLOR_MASK) == COLOR_GRAY ||
                                           (_mode & COLOR_MASK) == COLOR_GRAY_MASK)
                                              ? IMREAD_GRAYSCALE
                                              : IMREAD_COLOR;
                               }());
        if (!validate_image(src_mat, "Source image decode failed", nullptr))
            return {};

        // 设置ROI区域
        setup_roi(src_mat, roi);
        if (src_mat.empty()) return {};

        // 尺寸验证
        if (target_mat.cols > src_mat.cols || target_mat.rows > src_mat.rows) {
            fprintf(stderr, "Target size (%dx%d) exceeds source ROI (%dx%d)\n",
                    target_mat.cols, target_mat.rows,
                    src_mat.cols, src_mat.rows);
            return {};
        }

        // 执行模板匹配
        Mat result;
        match_template(src_mat, result);

        // 处理匹配结果
        std::vector<Rect> boxes;
        std::vector<float> probs;
        result.forEach<float>([&](float val, const int *pos) {
            const bool is_valid = (_mode & MODEL_MASK) == MODEL_SQDIFF_NORMED
                                      ? (val < 1.0f - prob_threshold)
                                      : (val > prob_threshold);

            if (is_valid) {
                boxes.emplace_back(pos[1] + roi.x, pos[0] + roi.y,
                                   target_mat.cols, target_mat.rows);
                probs.push_back(val);
            }
        });

        // 非极大值抑制
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, probs, prob_threshold, nms_threshold, indices);

        // 构建最终结果
        std::vector<objectEx> res;
        res.reserve(indices.size());
        for (int idx: indices) {
            res.push_back({boxes[idx], probs[idx]});
        }

        return res;
    }

    void template_matcher::setup_roi(cv::Mat &src, cv::Rect &roi) const {
        // 自动校正ROI参数
        if (roi.empty()) {
            roi = Rect(0, 0, src.cols, src.rows);
            return;
        }

        // 边界约束
        roi.x = std::max(roi.x, 0);
        roi.y = std::max(roi.y, 0);
        roi.width = std::min(roi.width, src.cols - roi.x);
        roi.height = std::min(roi.height, src.rows - roi.y);

        if (roi.area() <= 0) {
            fprintf(stderr, "Invalid ROI: x=%d, y=%d, w=%d, h=%d\n",
                    roi.x, roi.y, roi.width, roi.height);
            src = Mat();
            return;
        }

        src = src(roi);
    }

    void template_matcher::match_template(const cv::Mat &src, cv::Mat &result) const {
        const int method = (_mode & MODEL_MASK) == MODEL_SQDIFF_NORMED ? TM_SQDIFF_NORMED : TM_CCOEFF_NORMED;

        if (mask_mat.empty()) {
            ::cv::matchTemplate(src, target_mat, result, method);
        } else {
            ::cv::matchTemplate(src, target_mat, result, method, mask_mat);
        }
    }

    feature_detector::feature_detector(uint8_t *input_img_data,
                                       int input_img_size,
                                       feature_config *config) {
        // 解码输入图像
        cv::Mat input_image_ = decode_image(input_img_data, input_img_size);
        if (input_image_.empty()) {
            throw std::runtime_error("Failed to decode input image");
        }

        input_size_ = cv::Size(input_image_.cols, input_image_.rows);

        // 根据配置创建特征检测器
        switch (config->detector_type) {
            case SIFT_MODE: {
                auto &sift_cfg = config->params.sift;
                auto detector = cv::SIFT::create(
                    sift_cfg.nfeatures,
                    sift_cfg.nOctaveLayers,
                    sift_cfg.contrastThreshold,
                    sift_cfg.edgeThreshold,
                    sift_cfg.sigma
                );
                detector->detectAndCompute(input_image_, cv::noArray(), keypoints_, descriptors_);
                break;
            }

            case ORB_MODE: {
                auto &orb_cfg = config->params.orb;
                auto detector = cv::ORB::create(
                    orb_cfg.nfeatures,
                    orb_cfg.scaleFactor,
                    orb_cfg.nlevels,
                    orb_cfg.edgeThreshold,
                    orb_cfg.firstLevel,
                    orb_cfg.WTA_K,
                    (cv::ORB::ScoreType) orb_cfg.scoreType,
                    orb_cfg.patchSize,
                    orb_cfg.fastThreshold
                );
                detector->detectAndCompute(input_image_, cv::noArray(), keypoints_, descriptors_);
                break;
            }

            case AKAZE_MODE: {
                auto &akaze_cfg = config->params.akaze;
                auto detector = cv::AKAZE::create(
                    (cv::AKAZE::DescriptorType) akaze_cfg.descriptor_type,
                    akaze_cfg.descriptor_size,
                    akaze_cfg.descriptor_channels,
                    (cv::KAZE::DiffusivityType) akaze_cfg.threshold,
                    akaze_cfg.nOctaves,
                    akaze_cfg.nOctaveLayers,
                    (cv::KAZE::DiffusivityType) akaze_cfg.diffusivity
                );
                detector->detectAndCompute(input_image_, cv::noArray(), keypoints_, descriptors_);

                // 处理最大点数限制
                if (akaze_cfg.max_points > 0 && keypoints_.size() > static_cast<size_t>(akaze_cfg.max_points)) {
                    keypoints_.resize(akaze_cfg.max_points);
                    descriptors_ = descriptors_.rowRange(0, akaze_cfg.max_points);
                }
                break;
            }

            default:
                throw std::runtime_error("Unsupported detector type");
        }
    }

    std::vector<std::vector<cv::Point2f> > feature_match(
        const feature_detector &query,
        const feature_detector &source,
        const match_config &config) {
        // 获取关键点和描述符
        const auto &kp2 = source.getKeypoints();
        const auto &desc2 = source.getDescriptors();
        const auto &kp1 = query.getKeypoints();
        const auto &desc1 = query.getDescriptors();

        if (kp1.empty() || kp2.empty() || desc1.empty() || desc2.empty()) {
            return {};
        }

        std::vector<cv::DMatch> matches;

        // 根据描述符类型创建匹配器
        cv::Ptr<cv::DescriptorMatcher> matcher;

        // 匹配器类型选择
        switch (config.matcher_type) {
            case MATCHER_BINARY: {
                const int norm_type = cv::NORM_HAMMING;
                matcher = cv::BFMatcher::create(norm_type);

                std::vector<cv::DMatch> forward_matches, backward_matches;

                // 双向匹配
                matcher->match(desc1, desc2, forward_matches);
                matcher->match(desc2, desc1, backward_matches);

                // 构建反向映射表
                std::vector<int> reverse_map(desc2.rows, -1);
                for (size_t i = 0; i < backward_matches.size(); ++i) {
                    const auto &m = backward_matches[i];
                    reverse_map[m.trainIdx] = i;
                }

                // 交叉验证
                for (const auto &m: forward_matches) {
                    if (m.trainIdx < 0 || m.trainIdx >= static_cast<int>(reverse_map.size()))
                        continue;

                    const int reverse_idx = reverse_map[m.queryIdx];
                    if (reverse_idx == -1) continue;

                    const auto &rm = backward_matches[reverse_idx];
                    if (rm.trainIdx == m.queryIdx &&
                        m.distance <= config.thresh.max_hanming_distance &&
                        rm.distance <= config.thresh.max_hanming_distance) {
                        matches.push_back(m);
                    }
                }
                break;
            }
            case MATCHER_FLOAT: {
                // 浮点描述符使用KD树
                matcher = cv::makePtr<cv::FlannBasedMatcher>();

                std::vector<std::vector<cv::DMatch> > knn_matches;
                const int k = 2; // 取前2个最近邻
                matcher->knnMatch(desc1, desc2, knn_matches, k);

                // 比率测试
                for (const auto &candidates: knn_matches) {
                    if (candidates.size() < 2) continue;

                    const auto &best = candidates[0];
                    const auto &second = candidates[1];

                    // 双重过滤条件
                    if (best.distance < config.thresh.ratio_thresh * second.distance) {
                        matches.push_back(best);
                    }
                }
                break;
            }
            default:
                throw std::runtime_error("Unsupported matcher type");
        }

        std::vector<cv::Mat> homographies;
        std::vector<std::vector<uchar> > masks;
        multi_ransac(matches, kp1, kp2, homographies, masks,
                     config.ransac_threshold, config.min_inliers, config.max_models);

        // 生成结果四边形
        std::vector<std::vector<cv::Point2f> > results;
        const cv::Size query_size = query.getImageSize();
        const std::vector<cv::Point2f> src_corners = {
            {0, 0},
            {float(query_size.width), 0},
            {float(query_size.width), float(query_size.height)},
            {0, float(query_size.height)}
        };

        for (const auto &H: homographies) {
            std::vector<cv::Point2f> dst_corners(4);
            cv::perspectiveTransform(src_corners, dst_corners, H);

            // 凸性检查
            if (!cv::isContourConvex(std::vector<cv::Point2f>(dst_corners.begin(), dst_corners.end()))) {
                continue;
            }

            results.push_back(dst_corners);
        }

        return results;
    }
} // namespace libmatch
