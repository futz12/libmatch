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

// recreate from https://github.com/sml2h3/ddddocr
// No ddddocr, no this code.

#ifndef DDDDREC_H
#define DDDDREC_H

namespace libmatch {
    class ddddrec {
    public:
        ddddrec(std::vector<uint8_t> &rec_bin, std::string &rec_param,
                         std::string &keylist, int num_thread, bool use_vulkan);

        std::tuple<float, std::string, std::vector<int> > detect(
            const cv::Mat &src, const std::string &options);

    private:
        ncnn::Net recNet;
        std::vector<std::string> keys;

        std::vector<uint8_t> _rec_bin;

        std::map<std::string, int> char2index;
    };
} // libmatch

#endif //DDDDREC_H
