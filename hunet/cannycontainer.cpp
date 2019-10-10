#include "cannycontainer.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QtMath>

SScCannyContainer::SScCannyContainer() : m_valid(false) {}
SScCannyContainer::SScCannyContainer(const QString& filename, const SScCannySetting& setting) : m_valid(false)
{
    cv::Mat orig = cv::imread(filename.toUtf8().constData(),cv::IMREAD_COLOR);
    m_valid = (orig.cols>0) && (orig.rows>0);
    if (m_valid)
    {
        if (setting.m_bil)
        {
            cv::bilateralFilter(orig,m_mat,setting.m_bil_d,setting.m_bil_csigma,setting.m_bil_sigma);
            cv::cvtColor(m_mat,m_mat,CV_BGR2GRAY);
        }
        else
        {
            cv::cvtColor(orig,m_mat,CV_BGR2GRAY);
            cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
            clahe->apply(m_mat,m_mat);
        }
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
        if (setting.m_clip)
        {
            bool first = true;
            uchar min=255, max=0;
            for (int y=0; y<m_mat.rows; ++y) for (int x=0; x<m_mat.cols; ++x) for (int c=0; c<m_mat.channels(); ++c)
            {
                const uchar gv = m_mat.at<cv::Vec3b>(y,x)[c];
                if (Q_UNLIKELY(first))
                {
                    min=gv;
                    max=gv;
                    first=false;
                }
                else
                {
                    if (gv<min) min=gv; else if (gv>max) max=gv;
                }
            }
            qWarning(">>>>TRY CLIP %d %d", (int)min,(int)max);
            if ((max-min)!=255)
            {
                qWarning(">>>>>>CLIP %d %d", (int)min,(int)max);
                const double alpha = (double)(255)/(double)(max-min), beta = -min;

                m_mat.convertTo(m_mat,-1,alpha,beta);
            }
            else qWarning("NO CLIP");
        }
        m_orig = SSnImageConverter::image(m_mat);
    }
}
QList<SScContour> SScCannyContainer::contours(double min, double max)
{
    QList<SScContour> ret;
    if ((min<max) && (min>=0) && (max<=255))
    {
        cv::Mat cmat;
        cv::Canny(m_mat,cmat,min,max,3,true);
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(cmat,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
        for (unsigned int i=0; i<contours.size(); ++i)
        {
            const SScContour c(contours[i]);
            if (c.size()>5) ret << c;
        }
        m_canny = SSnImageConverter::image(cmat);
    }
    return ret;
}

int SScCannyContainer::diag() const
{
    return qSqrt(qPow(m_mat.cols,2.0)+qPow(m_mat.rows,2.0));
}
