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

#include <vector>
#include <map>

#include <cstdint>
#include <opencv2/opencv.hpp>
#include <ncnn/net.h>
#include <ncnn/mat.h>

#include "ddddrec.h"

namespace libmatch {
    ddddrec::ddddrec(std::vector<uint8_t> &rec_bin, std::string &rec_param, std::string &keylist, int num_thread,
                     bool use_vulkan) {
        recNet.opt.use_vulkan_compute = use_vulkan;
        recNet.opt.num_threads = num_thread;

        recNet.load_param_mem(rec_param.data());
        _rec_bin = rec_bin; // copy
        recNet.load_model(_rec_bin.data());

        std::stringstream ss(keylist);
        std::string line;
        int count = 0;
        while (std::getline(ss, line)) {
            keys.emplace_back(line);
            char2index[line] = count++;
        }
    }

    std::tuple<float, std::string, std::vector<int> > ddddrec::detect(const cv::Mat &src, const std::string &options) {
        // judge gray or not
        cv::Mat img_gray;
        if (src.channels() == 3) {
            cv::cvtColor(src, img_gray, cv::COLOR_BGR2GRAY);
        } else if (src.channels() == 4) {
            cv::cvtColor(src, img_gray, cv::COLOR_BGRA2GRAY);
        } else {
            img_gray = src.clone();
        }

        int width = img_gray.cols;

        ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_gray.data, ncnn::Mat::PIXEL_GRAY, img_gray.cols, img_gray.rows,
                                                     img_gray.cols / (float) img_gray.rows * 64, 64);
        const float mean_vals[1] = {127.5};
        const float norm_vals[1] = {1 / 127.5};
        in.substract_mean_normalize(mean_vals, norm_vals);

        auto ex = recNet.create_extractor();
        ex.input("in0", in);
        ncnn::Mat out;
        ex.extract("out0", out);

        int len = out.h;
        int vocab_size = out.w;

        std::string result;
        std::vector<int> charPositions;
        float prob = 1.0f;

        if (options.empty()) {
            // default: full character
            std::string last_item;
            for (int i = 0; i < len; i++) {
                int maxIndex = 0;
                float maxValue = -1.1f;
                for (int j = 0; j < vocab_size; j++) {
                    if (out.row(i)[j] > maxValue) {
                        maxIndex = j;
                        maxValue = out.row(i)[j];
                    }
                }
                if (maxIndex != 0 && keys[maxIndex] != last_item) {
                    result += keys[maxIndex];
                    charPositions.push_back(i / (float) len * width);
                    prob *= maxValue;
                }
            }
        } else {
            // filter by options
            std::vector<int> filter;
            filter.push_back(0); // blank
            std::stringstream ss(options);
            std::string item;
            while (std::getline(ss, item)) {
                if (char2index.find(item) != char2index.end()) {
                    filter.push_back(char2index[item]);
                }
            }

            std::string last_item;
            for (int i = 0; i < len; i++) {
                int maxIndex = 0;
                float maxValue = -0.1f;
                float sumValue = 0;
                for (auto x: filter) {
                    if (out.row(i)[x] > maxValue) {
                        maxIndex = x;
                        maxValue = out.row(i)[x];
                        sumValue += maxValue;
                    }
                }
                if (maxIndex != 0 && keys[maxIndex] != last_item) {
                    result += keys[maxIndex];
                    charPositions.push_back(i / (float) len * width);
                    prob *= maxValue / sumValue;
                }
            }
        }
        return std::make_tuple(prob, result, charPositions);
    }
} // libmatch
