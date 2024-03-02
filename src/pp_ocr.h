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


// this code comes from https://github.com/FeiGeChuanShu/ncnn_paddleocr

#ifndef LIBMATCH_PP_OCR_H
#define LIBMATCH_PP_OCR_H


namespace libmatch {

    template<class ForwardIterator>
    inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
        return std::distance(first, std::max_element(first, last));
    }

    struct TextLine {
        std::string text;
        std::vector<float> charScores;
        std::vector<float> charPositions;//每个字符的位置
    };
    struct Angle {
        int index;
        float score;
    };
    struct TextBox {
        std::vector<cv::Point> boxPoint;
        float score;
        std::string text;
        std::vector<int> charPositions; // 绝对位置 x 坐标
    };

    class ppocr {
    public:
        ppocr::ppocr(std::vector<uint8_t> &det_bin, std::string &det_param, std::vector<uint8_t> &rec_bin,
                     std::string &rec_param,
                     std::string &keylist, int num_thread, bool use_vulkan);

        std::vector<TextBox> detect(cv::Mat &src);

    private:
        ncnn::Net dbNet;
        ncnn::Net crnnNet;
        std::vector<std::string> keys;

        std::vector<uint8_t> _det_bin , _rec_bin;

        int _num_thread;

        const int dstHeight = 48;

        inline std::vector<cv::Point>
        getMinBoxes(const std::vector<cv::Point> &inVec, float &minSideLen, float &allEdgeSize);

        inline float boxScoreFast(const cv::Mat &inMat, const std::vector<cv::Point> &inBox);

        inline std::vector<cv::Point> unClip(const std::vector<cv::Point> &inBox, float perimeter, float unClipRatio);

        inline std::vector<TextBox> findRsBoxes(const cv::Mat &fMapMat, const cv::Mat &norfMapMat,
                                                const float boxScoreThresh, const float unClipRatio);

        inline std::vector<TextBox>
        getTextBoxes(const cv::Mat &src, float boxScoreThresh, float boxThresh, float unClipRatio);

        inline TextLine scoreToTextLine(const std::vector<float> &outputData, int h, int w);

        inline TextLine getTextLine(const cv::Mat &src);

        inline std::vector<TextLine> getTextLines(std::vector<cv::Mat> &partImg);

        inline cv::Mat getRotateCropImage(const cv::Mat &src, std::vector<cv::Point> box);

        inline std::vector<cv::Mat> getPartImages(const cv::Mat &src, std::vector<TextBox> &textBoxes);
    };

} // libmatch

#endif //LIBMATCH_PP_OCR_H
