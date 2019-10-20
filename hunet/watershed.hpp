#ifndef WATERSHED_HPP
#define WATERSHED_HPP

#include <opencv2/core.hpp>

namespace SSnWatershed
{
    void execute(cv::Mat& src, cv::Mat& trg, int thr = 40);
}
#endif // WATERSHED_HPP
