//
// Created by Ice on 2024/3/1.
//

#ifndef LIBMATCH_BASE_MATCH_H
#define LIBMATCH_BASE_MATCH_H


namespace libmatch {


#define COLOR_MASK        0x0000000F

#define COLOR_GRAY         0x00000000
#define COLOR_BGRA         0x00000001
#define COLOR_BGR          0x00000002
#define COLOR_BGRA_COLOR   0x00000003 // 真彩模式

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
        compute(uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold);
    }; // 模板匹配匹配子

    class orb_matcher {
    private:
        uint32_t _mode;
        std::vector<cv::KeyPoint> target_kps;
        cv::Mat target_desc;

        int target_img_width;
        int target_img_height;

    public:
        orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode);

        bool compute(uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, objectEx2 *res);
    };
} // libmatch

#endif //LIBMATCH_BASE_MATCH_H
