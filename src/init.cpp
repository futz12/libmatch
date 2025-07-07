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
#include <opencv2/core/ocl.hpp>
#include <MNN/MNNDefine.h>

#include "define.h"


struct {
    int flag = []()->int {
        printf(""
            "==============================\n"
            "           LibMatch           \n"
            "     BUG反馈 Q群: 767178345    \n"
            "    GitHub: Futz12/libmatch   \n"
            "     本项目以MIT开源,免费使用    \n"
            "   作者对程序造成的后果一概不负责  \n"
            "==============================\n"
            "程序版本: %s\n"
            "使用类库版本\n"
            "+  MNN: %s\n"
            "+  OpenCV: %s\n"
            ,LIBMATCH_VERSION,MNN_VERSION,CV_VERSION);

        printf("环境检查\n");

        // OpenCL 检查
        if (cv::ocl::useOpenCL()) {
            printf("+   OpenCL: 已启用\n");
        } else {
            printf("+   OpenCL: 未启用\n");
        }
        // 获取 OpenCL 设备信息 ([设备类型] 设备名称 - OpenCL 版本)
        std::vector<cv::ocl::PlatformInfo> platformsInfos;
        cv::ocl::getPlatfomsInfo(platformsInfos);
        printf("+   OpenCL 平台数量: %zu\n", platformsInfos.size());
        for (int i = 0; i < platformsInfos.size(); i++) {
            const cv::ocl::PlatformInfo& platformInfo = platformsInfos[i];
            printf("[平台 %d] %s - %s\n", i, platformInfo.name().c_str(), platformInfo.version().c_str());
            printf("设备数量: %d\n", platformInfo.deviceNumber());

            int deviceCount = platformInfo.deviceNumber();
            for (int j = 0; j < deviceCount; j++) {
                cv::ocl::Device device;
                platformInfo.getDevice(device, j);
                printf("  [设备 %d] %s - %s\n", j, device.name().c_str(), device.version().c_str());
            }
        }
        return 0;
    }();
} init;