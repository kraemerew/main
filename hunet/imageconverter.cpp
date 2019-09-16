#include "imageconverter.h"

QImage SSnImageConverter::image(const cv::Mat& mat)
{
    QImage ret = QImage(QSize(mat.cols,mat.rows), QImage::Format_Grayscale8);

    for (int i=0; i<mat.rows; ++i)
        std::memcpy(ret.scanLine(i),mat.row(i).data,mat.cols);

    return ret;
}
