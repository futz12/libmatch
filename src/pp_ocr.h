//
// Created by Ice on 2024/3/1.
//

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
