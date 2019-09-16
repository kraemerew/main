#ifndef CANNYCONTAINER_HPP
#define CANNYCONTAINER_HPP

#include <QImage>
#include <opencv2/core.hpp>
#include "contour.hpp"
#include "imageconverter.h"

class SScCannySetting
{
public:
    /*!
     * \brief SScCannySetting
     * \param median    Median in percentage of diagonal (0..100)
     * \param eq        Use histogram equalisation
     */
    SScCannySetting(double median = 1.0, bool eq = false)
        : m_median(qBound(0.0,median,100.0)), m_eq(eq) {}
    double m_median;
    bool    m_eq;
};

class SScCannyContainer
{
public:
    SScCannyContainer();
    SScCannyContainer(const QString& filename, const SScCannySetting& setting = SScCannySetting());
    QList<SScContour> contours(double min, double max);

    inline bool   isValid   () const { return m_valid; }
    inline QImage orig      () const { return SSnImageConverter::image(m_mat); }
    inline QImage canny     () const { return SSnImageConverter::image(m_cmat); }

private:
    QImage image(const cv::Mat& mat) const;
    bool m_valid;
    cv::Mat m_mat;
    cv::Mat m_cmat;
};

#endif // CANNYCONTAINER_HPP
