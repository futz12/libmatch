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
