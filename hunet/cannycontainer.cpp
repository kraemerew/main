#include "cannycontainer.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QtMath>

SScCannyContainer::SScCannyContainer() : m_valid(false) {}
SScCannyContainer::SScCannyContainer(const QString& filename, const SScCannySetting& setting) : m_valid(false)
{
    cv::Mat orig = cv::imread(filename.toUtf8().constData(),cv::IMREAD_COLOR);
    if (setting.m_bil)
    {
        cv::bilateralFilter(orig,m_mat,setting.m_bil_d,setting.m_bil_csigma,setting.m_bil_sigma);
        cv::cvtColor(m_mat,m_mat,CV_BGR2GRAY);
    }
    else
    {
        cv::cvtColor(orig,m_mat,CV_BGR2GRAY);
    }
    m_valid = (m_mat.cols>0) && (m_mat.rows>0);
    if (m_valid)
    {
        const double diag = qSqrt(qPow(m_mat.rows,2)+qPow(m_mat.cols,2.0));
        int median = qRound((setting.m_median*diag)/100.0);
        if (median%2==0) ++median;

        if (median>2)
        {
            if (median%2==0) ++median;
            cv::medianBlur(m_mat,m_mat,median);
        }
        if (setting.m_eq)
        {
            cv::equalizeHist(m_mat,m_mat);
        }
        else if (setting.m_clip)
        {
            uchar min=255, max=0;
            for (int y=0; y<m_mat.rows; ++y) for (int x=0; x<m_mat.cols; ++x) for (int c=0; c<m_mat.channels(); ++c)
            {
                const uchar gv = m_mat.at<cv::Vec3b>(y,x)[c];
                if (gv<min) min=gv; else if (gv>max) max=gv;
            }
            if ((min!=0) || (max!=255))
            {
                const double alpha = (double)(255)/(max-min), beta = -min;
                m_mat.convertTo(m_mat,-1,alpha,beta);
            }
        }
        /*else
        {
            //cv::morphologyEx(m_mat,m_mat,cv::MORPH_HITMISS, cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(median,median)));
            //cv::morphologyEx(m_mat,m_mat,cv::MORPH_CLOSE,cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(median,median)));

            auto clp = cv::createCLAHE(40.0, cv::Size(8,8));
            clp->apply(m_mat,m_mat);
        }*/
    }
}
QList<SScContour> SScCannyContainer::contours(double min, double max)
{
    QList<SScContour> ret;
    if ((min<max) && (min>=0) && (max<=255))
    {
        cv::Canny(m_mat,m_cmat,min,max,3,true);
        //cv::morphologyEx(m_cmat,m_cmat,cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(5,5)));

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(m_cmat,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
        for (unsigned int i=0; i<contours.size(); ++i)
        {
            const SScContour c(contours[i]);
            if (c.size()>5) ret << c;
        }
    }
    return ret;
}

int SScCannyContainer::diag() const
{
    return qSqrt(qPow(m_mat.cols,2.0)+qPow(m_mat.rows,2.0));
}
