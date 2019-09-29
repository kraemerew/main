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
    SScCannySetting(double median = 1.0, bool eq = false, bool bil = true, int d = 1, double csigma=1, double sigma = 2)
        :   m_eq(eq),
            m_bil(bil),
            m_bil_d(d),
            m_median(qBound(0.0,median,100.0)),
            m_bil_csigma(csigma),
            m_bil_sigma(sigma) {}
    bool    m_eq, m_bil;
    int     m_bil_d;
    double  m_median;
    double  m_bil_csigma, m_bil_sigma;
};

class SScCannyContainer
{
public:
    SScCannyContainer();
    SScCannyContainer(const QString& filename, const SScCannySetting& setting = SScCannySetting());
    QList<SScContour> contours(double min, double max);

           int    diag      () const;
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
