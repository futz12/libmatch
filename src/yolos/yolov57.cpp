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

#include <opencv2/opencv.hpp>
#include <ncnn/net.h>
#include <json.hpp>
#include "../base_algorithm.h"
#include "yolov57.h"

namespace libmatch {

    void
    yolo57::generate_proposals(const ncnn::Mat &anchors, int stride, const ncnn::Mat &in_pad, const ncnn::Mat &feat_blob,
                       float prob_threshold, std::vector<object> &objects) {
        const int num_grid = feat_blob.h;

        int num_grid_x;
        int num_grid_y;
        if (in_pad.w > in_pad.h) {
            num_grid_x = in_pad.w / stride;
            num_grid_y = num_grid / num_grid_x;
        } else {
            num_grid_y = in_pad.h / stride;
            num_grid_x = num_grid / num_grid_y;
        }

        const int num_class = feat_blob.w - 5;

        const int num_anchors = anchors.w / 2;

        for (int q = 0; q < num_anchors; q++) {
            const float anchor_w = anchors[q * 2];
            const float anchor_h = anchors[q * 2 + 1];

            const ncnn::Mat feat = feat_blob.channel(q);

            for (int i = 0; i < num_grid_y; i++) {
                for (int j = 0; j < num_grid_x; j++) {
                    const float *featptr = feat.row(i * num_grid_x + j);
                    float box_confidence = sigmoid(featptr[4]);
                    if (box_confidence >= prob_threshold) {
                        // find class index with max class score
                        int class_index = 0;
                        float class_score = -FLT_MAX;
                        for (int k = 0; k < num_class; k++) {
                            float score = featptr[5 + k];
                            if (score > class_score) {
                                class_index = k;
                                class_score = score;
                            }
                        }
                        float confidence = box_confidence * sigmoid(class_score);
                        if (confidence >= prob_threshold) {
                            // yolov5/models/yolo.py Detect forward
                            // y = x[i].sigmoid()
                            // y[..., 0:2] = (y[..., 0:2] * 2. - 0.5 + self.grid[i].to(x[i].device)) * self.stride[i]  # xy
                            // y[..., 2:4] = (y[..., 2:4] * 2) ** 2 * self.anchor_grid[i]  # wh

                            float dx = sigmoid(featptr[0]);
                            float dy = sigmoid(featptr[1]);
                            float dw = sigmoid(featptr[2]);
                            float dh = sigmoid(featptr[3]);

                            float pb_cx = (dx * 2.f - 0.5f + j) * stride;
                            float pb_cy = (dy * 2.f - 0.5f + i) * stride;

                            float pb_w = pow(dw * 2.f, 2) * anchor_w;
                            float pb_h = pow(dh * 2.f, 2) * anchor_h;

                            float x0 = pb_cx - pb_w * 0.5f;
                            float y0 = pb_cy - pb_h * 0.5f;
                            float x1 = pb_cx + pb_w * 0.5f;
                            float y1 = pb_cy + pb_h * 0.5f;

                            object obj;
                            obj.rect.x = x0;
                            obj.rect.y = y0;
                            obj.rect.width = x1 - x0;
                            obj.rect.height = y1 - y0;
                            obj.label = class_index;
                            obj.prob = confidence;

                            objects.push_back(obj);
                        }
                    }
                }
            }
        }
    }

    yolo57::yolo57(std::vector<uint8_t> &bin, std::string &param, std::string &config) {
        cfg = configor::json::parse(config);

        std::string name = cfg["name"];
        std::string version = cfg["version"];
        bool use_gpu = cfg["use_gpu"];\
            int use_cpu_thread = cfg["use_cpu_thread"];
        bool use_fp16_packed = cfg["use_fp16_packed"];
        bool use_fp16_storage = cfg["use_fp16_storage"];
        bool use_fp16_arithmetic = cfg["use_fp16_arithmetic"];
        bool light_mode = cfg["light_mode"];

        net = new ncnn::Net();
        net->opt.use_vulkan_compute = use_gpu;
        net->opt.use_fp16_packed = use_fp16_packed;
        net->opt.use_fp16_storage = use_fp16_storage;
        net->opt.use_fp16_arithmetic = use_fp16_arithmetic;
        net->opt.num_threads = use_cpu_thread;
        net->opt.lightmode = light_mode;

        if (use_gpu)
            net->set_vulkan_device(cfg["gpu_device_id"].get<int>());

        net->load_param_mem(param.c_str());
        _bin = bin; // 因为ncnn load是浅拷贝，所以需要保存bin数据
        net->load_model(_bin.data());
    }

    yolo57::~yolo57() {
        delete net;
    }

    std::vector<object>
    yolo57::detect(uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold, bool agnostic) {
        int channels = cfg["channels"];
        int width = cfg["width"];
        int height = cfg["height"];
        int max_stride = cfg["MAX_STRIDE"];
        int pixel_type = cfg["pixels_type"];


        cv::_InputArray image_io(src_img_data, src_img_size);
        cv::Mat image = cv::imdecode(image_io, cv::IMREAD_UNCHANGED);

        int img_w = image.cols;
        int img_h = image.rows;

        float scale = 1.f;

        int w = img_w;
        int h = img_h;
        if (w > h) {
            scale = (float) width / w;
            w = width;
            h = h * scale;
        } else {
            scale = (float) height / h;
            h = height;
            w = w * scale;
        }

        ncnn::Mat in = ncnn::Mat::from_pixels_resize(image.data, pixel_type, img_w, img_h, w, h);

        int wpad = (w + max_stride - 1) / max_stride * max_stride - w;
        int hpad = (h + max_stride - 1) / max_stride * max_stride - h;
        ncnn::Mat in_pad;
        ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2,
                               ncnn::BORDER_CONSTANT,
                               cfg["border_value"]);

        std::shared_ptr<float[]> norm_vals;

        if (cfg["skip_norm"] == false) {
            if (pixel_type & (ncnn::Mat::PIXEL_GRAY << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                norm_vals = std::shared_ptr<float[]>(new float[1]{cfg["norm_vals"][0]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_RGB << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                norm_vals = std::shared_ptr<float[]>(
                        new float[3]{cfg["norm_vals"][0], cfg["norm_vals"][1], cfg["norm_vals"][2]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_BGR << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                norm_vals = std::shared_ptr<float[]>(
                        new float[3]{cfg["norm_vals"][2], cfg["norm_vals"][1], cfg["norm_vals"][0]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_RGBA << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                norm_vals = std::shared_ptr<float[]>(
                        new float[4]{cfg["norm_vals"][0], cfg["norm_vals"][1], cfg["norm_vals"][2],
                                     cfg["norm_vals"][3]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_BGRA << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                norm_vals = std::shared_ptr<float[]>(
                        new float[4]{cfg["norm_vals"][2], cfg["norm_vals"][1], cfg["norm_vals"][0],
                                     cfg["norm_vals"][3]});
            } else {
                throw std::runtime_error("Unsupported pixel type");
            }
        } else {
            norm_vals = std::shared_ptr<float[]>(nullptr);
        }

        std::shared_ptr<float[]> mean_vals;

        if (cfg["skip_mean"] == false) {
            if (pixel_type & (ncnn::Mat::PIXEL_GRAY << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                mean_vals = std::shared_ptr<float[]>(new float[1]{cfg["mean_vals"][0]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_RGB << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                mean_vals = std::shared_ptr<float[]>(
                        new float[3]{cfg["mean_vals"][0], cfg["mean_vals"][1], cfg["mean_vals"][2]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_BGR << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                mean_vals = std::shared_ptr<float[]>(
                        new float[3]{cfg["mean_vals"][2], cfg["mean_vals"][1], cfg["mean_vals"][0]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_RGBA << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                mean_vals = std::shared_ptr<float[]>(
                        new float[4]{cfg["mean_vals"][0], cfg["mean_vals"][1], cfg["mean_vals"][2],
                                     cfg["mean_vals"][3]});
            } else if (pixel_type & (ncnn::Mat::PIXEL_BGRA << ncnn::Mat::PIXEL_CONVERT_SHIFT)) {
                mean_vals = std::shared_ptr<float[]>(
                        new float[4]{cfg["mean_vals"][2], cfg["mean_vals"][1], cfg["mean_vals"][0],
                                     cfg["mean_vals"][3]});
            } else {
                throw std::runtime_error("Unsupported pixel type");
            }
        } else {
            mean_vals = std::shared_ptr<float[]>(nullptr);
        }

        in_pad.substract_mean_normalize(mean_vals.get(), norm_vals.get());

        ncnn::Extractor ex = net->create_extractor();
        ex.input(((std::string) (cfg["input_layer"])).c_str(), in_pad);

        std::vector<object> proposals;

        if (cfg["end2end"]) {
            ncnn::Mat out;
            ex.extract(((std::string) cfg["output_layers"]).c_str(), out);

            for (int i = 0; i < out.h; i++) {
                const float *values = out.row(i);

                object obj;
                obj.label = values[0];
                obj.prob = values[1];

                obj.rect.x = (values[2] * width - (wpad / 2)) / w * img_w;
                obj.rect.y = (values[3] * height - (hpad / 2)) / h * img_h;


                obj.rect.width = (values[4] - values[2]) * width / w * img_w;
                obj.rect.height = (values[5] - values[3]) * height / h * img_h;

                proposals.push_back(obj);
            }
            return proposals;
        }

        for (int i = 0; i < cfg["output_layers"].size(); i++) {
            configor::value output_layer = cfg["output_layers"][i];

            ncnn::Mat out;
            ex.extract(((std::string) output_layer["layer"]).c_str(), out);
            ncnn::Mat anchors(6);

            for (int j = 0; j < 6; j++) {
                anchors[j] = output_layer["anchors"][j];
            }
            std::vector<object> objects_tmp;

            generate_proposals(anchors, output_layer["stride"], in_pad, out, prob_threshold, objects_tmp);
            proposals.insert(proposals.end(), objects_tmp.begin(), objects_tmp.end());
        }

        std::vector<int> picked;
        nms_sorted_bboxes(proposals, picked, nms_threshold, agnostic);

        int count = picked.size();

        std::vector<object> objects;

        objects.resize(count);
        for (int i = 0; i < count; i++) {
            objects[i] = proposals[picked[i]];

            // adjust offset to original unpadded
            float x0 = (objects[i].rect.x - (wpad / 2)) / scale;
            float y0 = (objects[i].rect.y - (hpad / 2)) / scale;
            float x1 = (objects[i].rect.x + objects[i].rect.width - (wpad / 2)) / scale;
            float y1 = (objects[i].rect.y + objects[i].rect.height - (hpad / 2)) / scale;

            // clip
            x0 = std::max(std::min(x0, (float) (img_w - 1)), 0.f);
            y0 = std::max(std::min(y0, (float) (img_h - 1)), 0.f);
            x1 = std::max(std::min(x1, (float) (img_w - 1)), 0.f);
            y1 = std::max(std::min(y1, (float) (img_h - 1)), 0.f);

            objects[i].rect.x = x0;
            objects[i].rect.y = y0;
            objects[i].rect.width = x1 - x0;
            objects[i].rect.height = y1 - y0;
        }

        return objects;
    }

} // libmatch