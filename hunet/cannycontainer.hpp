#ifndef CANNYCONTAINER_HPP
#define CANNYCONTAINER_HPP

#include <QImage>
#include <opencv2/core.hpp>
#include "contour.hpp"
#include "imageconverter.h"

class SScCannySetting
{
public:
    SScCannySetting(double median = 1.0, bool eq = false, bool clip = true, bool bil = true, int d = 1, double csigma=1, double sigma = 2)
        :   m_eq(eq),
            m_clip(clip),
            m_bil(bil),
            m_bil_d(d),
            m_median(qBound(0.0,median,100.0)),
            m_bil_csigma(csigma),
            m_bil_sigma(sigma)
    {}

    bool    m_eq, m_clip, m_bil;
    int     m_bil_d;
    double  m_median, m_bil_csigma, m_bil_sigma;
};

class SScCannyContainer
{
public:
    SScCannyContainer();
    SScCannyContainer(const QString& filename, const SScCannySetting& setting = SScCannySetting());
    QList<SScContour> contours(double min, double max);

           int    diag      () const;
    inline bool   isValid   () const { return m_valid; }
    inline QImage orig      () const { return m_orig; }
    inline QImage canny     () const { return m_canny; }

private:
    QImage image(const cv::Mat& mat) const;
    bool m_valid;
    QImage m_orig, m_canny;
    cv::Mat m_mat;
};

#endif // CANNYCONTAINER_HPP
