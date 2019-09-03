#include <QApplication>
#include <QElapsedTimer>
#include "highwaynn//network.hpp"
#include "convunit.hpp"
#include <QTimer>
#include <QImage>
#include "image/convimageconverter.hpp"
#include "blasvector.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif

class SScContour
{
public:
    SScContour(const std::vector<cv::Point>& v) : m_data(v)
    {
    }
private:
    std::vector<cv::Point> m_data;
};

    class SScCannyContainer
    {
    public:

        SScCannyContainer(const QString& filename, int median = 5, bool eq = true)
        {
            m_mat = cv::imread(filename.toUtf8().constData(),cv::IMREAD_GRAYSCALE);
            if ((median>1) && (median%2!=0)) cv::medianBlur(m_mat,m_mat,median);
            if (eq) cv::equalizeHist(m_mat,m_mat);
        }
       QImage orig() const { return image(m_mat); }
       QImage canny() const { return image(m_cmat); }
        QList<SScContour> contours(double min, double max)
        {
            QList<SScContour> ret;
            if ((min<max) && (min>=0) && (max<=255))
            {
                cv::Canny(m_mat,m_cmat,min,max);
                std::vector<std::vector<cv::Point> > contours;
                std::vector<cv::Vec4i> hierarchy;
                cv::findContours(m_cmat,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
                for (unsigned int i=0; i<contours.size(); ++i) ret << SScContour(contours[i]);
            }
            return ret;
        }

    private:
        QImage image(const cv::Mat& mat) const
        {
            QImage ret = QImage(QSize(mat.cols,mat.rows), QImage::Format_Grayscale8);

            for (int i=0; i<mat.rows; ++i)
                std::memcpy(ret.scanLine(i),mat.row(i).data,mat.cols);

            return ret;
        }
        cv::Mat m_mat;
        cv::Mat m_cmat;
    };

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    SScCannyContainer cc("/home/developer/00.jpg",21,false);
    auto cont = cc.contours(200,255);
    qWarning(">>>GOT %d contours", cont.size());
    cc.orig().save("/home/developer/mat.png");
    cc.canny().save("/home/developer/cmat.png");

    return 0;

}
