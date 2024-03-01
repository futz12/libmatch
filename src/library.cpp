#include <opencv2/opencv.hpp>
#include "library.h"

const char* version = "0.9.5";

LIBMATCH_API const char* libmatch_version()
{
    cv::getBuildInformation();
    return version;
}