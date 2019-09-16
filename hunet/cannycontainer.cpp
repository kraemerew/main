#include "cannycontainer.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QtMath>

SScCannyContainer::SScCannyContainer() : m_valid(false) {}
SScCannyContainer::SScCannyContainer(const QString& filename, const SScCannySetting& setting) : m_valid(false)
{
    m_mat = cv::imread(filename.toUtf8().constData(),cv::IMREAD_GRAYSCALE);
    m_valid = (m_mat.cols>0) && (m_mat.rows>0);
    if (m_valid)
    {
        const double diag = qSqrt(qPow(m_mat.rows,2)+qPow(m_mat.cols,2.0));
        int median = qRound((setting.m_median*diag)/100.0);
        if (median>1)
        {
            if (median%2==0) ++median;
            cv::medianBlur(m_mat,m_mat,median);
        }
        if (setting.m_eq)
        {
            cv::equalizeHist(m_mat,m_mat);
        }
    }
}
QList<SScContour> SScCannyContainer::contours(double min, double max)
{
    QList<SScContour> ret;
    if ((min<max) && (min>=0) && (max<=255))
    {
        cv::Canny(m_mat,m_cmat,min,max);
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(m_cmat,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
        for (unsigned int i=0; i<contours.size(); ++i)
        {
            const SScContour c(contours[i]);
            if ((c.size()>10) && (c.diag()>=diag()/10)) ret << c;
        }
    }
    return ret;
}

int SScCannyContainer::diag() const
{
    return qSqrt(qPow(m_mat.cols,2.0)+qPow(m_mat.rows,2.0));
}
