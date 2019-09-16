#ifndef CANNYCONTAINER_HPP
#define CANNYCONTAINER_HPP

#include <QImage>
#include <opencv2/core.hpp>
#include "contour.hpp"

class SScCannyContainer
{
public:
    SScCannyContainer();
    SScCannyContainer(const QString& filename, int median = 5, bool eq = true);
    QList<SScContour> contours(double min, double max);

    inline bool   isValid   () const { return m_valid; }
    inline QImage orig      () const { return image(m_mat); }
    inline QImage canny     () const { return image(m_cmat); }


private:
    QImage image(const cv::Mat& mat) const;
    bool m_valid;
    cv::Mat m_mat;
    cv::Mat m_cmat;
};

#endif // CANNYCONTAINER_HPP
