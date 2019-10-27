#ifndef CANNYCONTAINER_HPP
#define CANNYCONTAINER_HPP

#include <QImage>
#include <opencv2/core.hpp>
#include "contour.hpp"
#include "imageconverter.h"
#include "sscvm.hpp"

class SScCannySetting : public SScVM
{
public:
    SScCannySetting(double median = 1.0, bool eq = false, bool clip = true, bool bil = true, int d = 1, double csigma=1, double sigma = 2)
        : SScVM()
    {
        insert("EQ",    eq);
        insert("CLIP",  clip);
        insert("BIL", bil);
        insert("MEDIAN", qBound(0.0,median,100.0));
        insert("BIL_SIGMA", sigma);
        insert("BIL_CSIGMA", csigma);
        insert("BID_DIST", d);
    }

    inline double   median() const { return doubleToken("MEDIAN",.1); }
    inline bool     eq() const { return boolToken("EQ",false); }
    inline bool     clip() const { return boolToken("CLIP",false); }
    inline bool     bil() const { return boolToken("BIL",false); }
    inline int      bil_d() const { return SScVM::intToken("BIL_DIST",1.0); }
    inline double   bil_csigma() const { return doubleToken("BIL_CSIGMA",1.0); }
    inline double   bil_sigma() const { return doubleToken("BIL_SIGMA",2.0); }
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
