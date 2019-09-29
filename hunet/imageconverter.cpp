#include "imageconverter.h"

QImage SSnImageConverter::image(const cv::Mat& mat)
{
    QImage ret;

    if (mat.channels()==1)
    {
        ret = QImage(QSize(mat.cols,mat.rows), QImage::Format_Grayscale8);
        for (int i=0; i<mat.rows; ++i)
            std::memcpy(ret.scanLine(i),mat.row(i).data,mat.cols);
    }
    else  if (mat.channels()==3)
    {
        ret = QImage(QSize(mat.cols,mat.rows), QImage::Format_RGB888);

        for (int i=0; i<mat.cols; ++i) for (int j=0; j<mat.rows; ++j)
        {
            const cv::Vec3b& bgr = mat.at<cv::Vec3b>(j, i);
            QColor c(bgr[2],bgr[1],bgr[0]);
            ret.setPixelColor(i,j,c);
        }
    }
    return ret;
}
