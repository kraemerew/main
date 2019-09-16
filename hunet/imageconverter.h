#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <opencv2/core.hpp>
#include <QImage>

namespace SSnImageConverter
{
QImage image(const cv::Mat& im);
}

#endif // IMAGECONVERTER_H
