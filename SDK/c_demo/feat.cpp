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

#include "c_api.h"
#include <iostream>
#include <fstream>
#include <vector>

std::vector<uint8_t> read_file(char *path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

char query[] = "../assets/10.png";
char source[] = "../assets/9.png";

int main() {
    std::vector<uint8_t> query_bin = read_file(query);
    std::vector<uint8_t> source_bin = read_file(source);

    feature_config config;
    config.detector_type = SIFT_MODE;
    config.params.sift.nfeatures = 20000;
    config.params.sift.nOctaveLayers = 3;
    config.params.sift.contrastThreshold = 0.04;
    config.params.sift.edgeThreshold = 3;
    config.params.sift.sigma = 1.6;

    void* query_detector = feat_create_detector(query_bin.data(), query_bin.size(), &config);
    void* source_detector = feat_create_detector(source_bin.data(), source_bin.size(), &config);

    match_config match_config;
    match_config.matcher_type = MATCHER_FLOAT;
    match_config.thresh.ratio_thresh = 0.7;
    match_config.ransac_threshold = 3.0;
    match_config.min_inliers = 10;
    match_config.max_models = 5;

    void* result = feat_match(query_detector, source_detector, &match_config);
    int result_size = feat_match_result_size(result);
    std::cout << "Result size: " << result_size << std::endl;

    for (int i = 0; i < result_size; ++i) {
        objectEx2 obj;
        feat_match_result_get(result, i, &obj);

        std::cout << "Result " << i << ": ";
        for (int j = 0; j < 4; ++j) {
            std::cout << "(" << obj.dots[j].x << ", " << obj.dots[j].y << ") ";
        }
    }

    release_feat_match_result(result);

    feat_release_detector(query_detector);
    feat_release_detector(source_detector);

}