//
// Created by Ice on 2024/3/1.
//
#include <opencv2/opencv.hpp>
#include <ncnn/net.h>
#include <cstring>

#include "base_algorithm.h"
#include "base_match.h"
#include "pp_ocr.h"
#include <json.hpp>
#include "./yolos/yolov57.h"
#include "./dxgi/screenshot.h"

#include "c_api.h"


LIBMATCH_C_API void *create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode) {
    return new libmatch::template_matcher(target_img_data, target_img_size, mode);
}

LIBMATCH_C_API void release_template_matcher(void *matcher) {
    delete (libmatch::template_matcher *) matcher;
}

LIBMATCH_C_API void *
template_matcher_compute(void *matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold,
                         float nms_threshold) {
    return new std::vector<libmatch::objectEx>(
            ((libmatch::template_matcher *) matcher)->compute(src_img_data, src_img_size, prob_threshold,
                                                              nms_threshold));
}

LIBMATCH_C_API void *
template_matcher_compute_rect(void *matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold,
                              float nms_threshold, int sx, int sy, int ex, int ey) {
    return new std::vector<libmatch::objectEx>(
            ((libmatch::template_matcher *) matcher)->compute(src_img_data, src_img_size, prob_threshold, nms_threshold,
                                                              sx, sy, ex, ey));
}

LIBMATCH_C_API size_t template_matcher_result_size(void *result) {
    return ((std::vector<libmatch::objectEx> *) result)->size();
}

LIBMATCH_C_API void template_matcher_result_get(void *result, size_t index, void *result_obj) {
    auto &obj = ((std::vector<libmatch::objectEx> *) result)->at(index);
    memcpy(result_obj, &obj, sizeof(libmatch::objectEx));
}

LIBMATCH_C_API void release_template_matcher_result(void *result) {
    delete (std::vector<libmatch::objectEx> *) result;
}

LIBMATCH_C_API void *create_orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode) {
    return new libmatch::orb_matcher(target_img_data, target_img_size, n_features, mode);
}

LIBMATCH_C_API void release_orb_matcher(void *matcher) {
    delete (libmatch::orb_matcher *) matcher;
}

LIBMATCH_C_API bool
orb_matcher_compute(void *matcher, uint8_t *src_img_data, int src_img_size, int n_features, int max_distance,
                    void *result) {
    return ((libmatch::orb_matcher *) matcher)->compute(src_img_data, src_img_size, n_features, max_distance,
                                                        (libmatch::objectEx2 *) result);
}

LIBMATCH_C_API void *
create_ppocr(uint8_t *det_bin, int det_bin_size, char *det_param, uint8_t *rec_bin, int rec_bin_size, char *rec_param,
             const char *keylist, int num_thread, bool use_vulkan) {
    std::vector<uint8_t> det_bin_v(det_bin, det_bin + det_bin_size);
    std::string det_param_v(det_param);
    std::vector<uint8_t> rec_bin_v(rec_bin, rec_bin + rec_bin_size);
    std::string rec_param_v(rec_param);
    std::string keylist_s(keylist);
    return new libmatch::ppocr(det_bin_v, det_param_v, rec_bin_v, rec_param_v, keylist_s, num_thread, use_vulkan);
}

LIBMATCH_C_API void release_ppocr(void *ppocr) {
    delete (libmatch::ppocr *) ppocr;
}

LIBMATCH_C_API void *ppocr_detect(void *ppocr, uint8_t *src_img_data, int src_img_size) {
    cv::Mat src = cv::imdecode(cv::Mat(1, src_img_size, CV_8U, src_img_data), cv::IMREAD_COLOR);
    return new std::vector<libmatch::TextBox>(((libmatch::ppocr *) ppocr)->detect(src));
}

LIBMATCH_C_API size_t ppocr_result_size(void *result) {
    return ((std::vector<libmatch::TextBox> *) result)->size();
}

LIBMATCH_C_API void ppocr_get_textbox(void *result, size_t index, void *result_obj) {
    TextBox box;
    auto &obj = ((std::vector<libmatch::TextBox> *) result)->at(index);
    for (int i = 0; i < 4; i++) {
        box.boxPoint[i].x = obj.boxPoint[i].x;
        box.boxPoint[i].y = obj.boxPoint[i].y;
    }
    box.score = obj.score;
    box.text = new char[obj.text.size() + 1];
    memcpy(box.text, obj.text.c_str(), obj.text.size());
    box.text[obj.text.size()] = '\0';
    box.charPositions = new int[obj.charPositions.size() + 1];
    memcpy(box.charPositions, obj.charPositions.data(), obj.charPositions.size() * sizeof(int));
    box.size_charPositions = obj.charPositions.size();

    memcpy(result_obj, &box, sizeof(TextBox));
}

LIBMATCH_C_API void release_ppocr_textbox(void *result) {
    auto res = (TextBox *) (result);
    delete[] res->text;
    delete[] res->charPositions;

}

LIBMATCH_C_API void release_ppocr_result(void *result) {
    delete (std::vector<libmatch::TextBox> *) result;
}

LIBMATCH_C_API void *create_yolo57(uint8_t *bin, int bin_size, char *param, char *config) {
    std::vector<uint8_t> bin_v(bin, bin + bin_size);
    std::string param_v(param);
    std::string config_v(config);
    return new libmatch::yolo57(bin_v, param_v, config_v);
}

LIBMATCH_C_API void release_yolo57(void *yolo57) {
    delete (libmatch::yolo57 *) yolo57;
}

LIBMATCH_C_API void *
yolo57_detect(void *yolo57, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold,
              bool agnostic) {
    return new std::vector<libmatch::object>(((libmatch::yolo57 *) yolo57)->detect(src_img_data, src_img_size,
                                                                                   prob_threshold, nms_threshold,
                                                                                   agnostic));
}

LIBMATCH_C_API size_t yolo57_result_size(void *result) {
    return ((std::vector<libmatch::object> *) result)->size();
}

LIBMATCH_C_API void yolo57_get_object(void *result, size_t index, void *result_obj) {
    auto &obj = ((std::vector<libmatch::object> *) result)->at(index);
    memcpy(result_obj, &obj, sizeof(object));
}

LIBMATCH_C_API void release_yolo57_result(void *result) {
    delete (std::vector<libmatch::object> *) result;
}

LIBMATCH_C_API void unregister_vulkan() {
    ncnn::destroy_gpu_instance();
}

LIBMATCH_C_API void* create_window_capture(void *hwnd, int sx, int sy, int ex, int ey) {
    return new libmatch::window_capture((HWND)hwnd, sx, sy, ex, ey);
}

LIBMATCH_C_API void release_window_capture(void *capture) {
    delete (libmatch::window_capture *) capture;
}

LIBMATCH_C_API void capture_update(void *capture) {
    return ((libmatch::window_capture *) capture)->update();
}

LIBMATCH_C_API int capture_get_bitmap(void *capture,void **bitmap) {
    *bitmap = ((libmatch::window_capture *) capture)->bitmap;
    return ((libmatch::window_capture *) capture)->bitmap_size;
}