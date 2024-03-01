//
// Created by Ice on 2024/3/1.
//

#include <cstdint>

#include <opencv2/opencv.hpp>
#include "base_algorithm.h"
#include "base_match.h"

namespace libmatch {
    template_matcher::template_matcher(uint8_t *target_img_data,int target_img_size, uint32_t mode)
    {
        cv::_InputArray target_img_arr(target_img_data, target_img_size);
        if((mode & COLOR_MASK) == COLOR_GRAY)
        {
            target_mat = cv::imdecode(target_img_arr, cv::IMREAD_GRAYSCALE);
            READ_OVER();
        }
        else if((mode & COLOR_MASK) == COLOR_BGRA || (mode & COLOR_MASK) == COLOR_BGRA_COLOR)
        {
            target_mat = cv::imdecode(target_img_arr, cv::IMREAD_UNCHANGED);
            READ_OVER();
            if(target_mat.channels() != 4)
            {
                fprintf(stderr,"[Match] Err target_img is not BGRA");
                return;
            }
            cv::Mat bgr[4];
            cv::split(target_mat, bgr);
            mask_mat = bgr[3];
            // 归一化 mask_mat (0 - 1)
            mask_mat.convertTo(mask_mat, CV_32FC1, 1.0 / 255);
            if ((mode & COLOR_MASK) == COLOR_BGRA)
                cv::cvtColor(target_mat,target_mat, cv::COLOR_BGRA2GRAY);
            else
                cv::cvtColor(target_mat,target_mat, cv::COLOR_BGRA2BGR);
        }
        else if((mode & COLOR_MASK) == COLOR_BGR)
        {
            target_mat = cv::imdecode(target_img_arr, cv::IMREAD_COLOR);
            READ_OVER();
        }
        else
        {
            fprintf(stderr,"[Match] Err mode");
            return;
        }
        _mode = mode;
    }
    std::vector<objectEx> template_matcher::compute(uint8_t *src_img_data,int src_img_size,float prob_threshold,float nms_threshold)
    {
        cv::Mat src_mat;
        cv::_InputArray src_img_arr(src_img_data, src_img_size);
        if((_mode & COLOR_MASK) == COLOR_GRAY)
        {
            src_mat = cv::imdecode(src_img_arr, cv::IMREAD_GRAYSCALE);
            READ_OVER_SRC();
        }
        else if((_mode & COLOR_MASK) == COLOR_BGRA)
        {
            src_mat = cv::imdecode(src_img_arr, cv::IMREAD_GRAYSCALE);
            READ_OVER_SRC();
        }
        else if((_mode & COLOR_MASK) == COLOR_BGR || (_mode & COLOR_MASK) == COLOR_BGRA_COLOR)
        {
            src_mat = cv::imdecode(src_img_arr, cv::IMREAD_COLOR);
            READ_OVER_SRC();
        }

        // 判断大小是否合法
        if (target_mat.cols > src_mat.cols || target_mat.rows > src_mat.rows)
        {
            fprintf(stderr,"[Match] Err target_img is bigger than src_img");
            return {};
        }
        cv::Mat result(src_mat.cols - target_mat.cols + 1, src_mat.rows - target_mat.rows + 1, CV_32FC1);

        if((_mode & COLOR_MASK) == COLOR_BGRA || (_mode & COLOR_MASK) == COLOR_BGRA_COLOR)
        {
            cv::matchTemplate(src_mat, target_mat, result, cv::TM_CCOEFF_NORMED, mask_mat);
        }
        else
        {
            cv::matchTemplate(src_mat, target_mat, result, cv::TM_CCOEFF_NORMED);
        }


        std::vector <objectEx> proposals;

        for (int i = 0; i < result.rows; ++i)
        {
            for (int j = 0; j < result.cols; ++j)
            {
                float prob = result.at<float>(i, j);
                if (prob > prob_threshold)
                {
                    objectEx proposal;
                    proposal.rect.x = j;
                    proposal.rect.y = i;
                    proposal.rect.width = target_mat.cols;
                    proposal.rect.height = target_mat.rows;
                    proposal.prob = prob;
                    proposals.push_back(proposal);
                }
            }
        }

        std::vector<int> picked;
        nms_sorted_bboxes(proposals, picked, nms_threshold);

        std::vector<objectEx> res;
        res.reserve(picked.size());

        for (auto x : picked)
        {
            res.push_back(proposals[x]);
        }

        return res;
    }

    orb_matcher::orb_matcher(uint8_t *target_img_data, int target_img_size, int n_features, uint32_t mode) {
        cv::_InputArray target_img_arr(target_img_data, target_img_size);
        cv::Mat target_mat = cv::imdecode(target_img_arr, cv::IMREAD_GRAYSCALE);
        READ_OVER();

        cv::Ptr<cv::ORB> orb = cv::ORB::create(n_features);
        orb->detect(target_mat, target_kps);
        orb->compute(target_mat, target_kps, target_desc);

        _mode = mode;
        target_img_width = target_mat.cols;
        target_img_height = target_mat.rows;
    }

    bool orb_matcher::compute(uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, objectEx2 *res){
        cv::_InputArray src_img_arr(src_img_data, src_img_size);
        cv::Mat src_mat = cv::imdecode(src_img_arr, cv::IMREAD_GRAYSCALE);
        if (src_mat.empty()) {
            fprintf(stderr, "[Match] Err Can`t Read Image");
            return false;
        }

        cv::Mat src_desc;
        std::vector<cv::KeyPoint> src_kps;

        cv::Ptr<cv::ORB> orb = cv::ORB::create(n_features);

        orb->detect(src_mat, src_kps);
        orb->compute(src_mat, src_kps, src_desc);

        cv::BFMatcher matcher(cv::NORM_HAMMING, true);//利用汉明距离的匹配器
        std::vector<cv::DMatch> matches;

        matcher.match(target_desc, src_desc, matches);

        std::vector<cv::Point2f> good_src_kps;
        std::vector<cv::Point2f> good_target_kps;

        for (auto &match: matches) {
            if (match.distance < max_distance) {
                good_src_kps.push_back(src_kps[match.trainIdx].pt);
                good_target_kps.push_back(target_kps[match.queryIdx].pt);
            }
        }

        if (good_src_kps.size() < 4) {
            fprintf(stderr, "[Match] Err not enough matches");
            return false;
        }

        cv::Mat H = Findhomography(good_target_kps,good_src_kps);

        std::vector<cv::Point2f> obj_corners(4);
        obj_corners[0] = cv::Point2f(0, 0);
        obj_corners[1] = cv::Point2f((float) target_img_width, 0);
        obj_corners[2] = cv::Point2f((float) target_img_width, (float) target_img_height);
        obj_corners[3] = cv::Point2f(0, (float) target_img_height);

        std::vector<cv::Point2f> scene_corners(4);
        cv::perspectiveTransform(obj_corners, scene_corners, H);

        for (int i = 0; i < 4; i++) {
            res->dots[i].x = scene_corners[i].x;
            res->dots[i].y = scene_corners[i].y;
        }

        return true;
    }
} // libmatch
