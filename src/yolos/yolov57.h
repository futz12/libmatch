//
// Created by Ice on 2024/3/2.
//

#ifndef LIBMATCH_YOLOV57_H
#define LIBMATCH_YOLOV57_H

namespace libmatch {

    class yolo57 {
    private:
        ncnn::Net *net;
        configor::value cfg;
        std::vector<uint8_t> _bin;

        static void
        generate_proposals(const ncnn::Mat &anchors, int stride, const ncnn::Mat &in_pad, const ncnn::Mat &feat_blob,
                           float prob_threshold, std::vector<object> &objects);

    public:
        yolo57(std::vector<uint8_t> &bin, std::string &param, std::string &config);

        ~yolo57();

        std::vector<object>
        detect(uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold, bool agnostic);
    };

} // libmatch

#endif //LIBMATCH_YOLOV57_H
