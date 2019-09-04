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
#include <QLabel>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif

class SScContour
{
public:
    SScContour(const std::vector<cv::Point>& v) : m_data(v), m_done(false)
    {
        huMoments();
    }

    double* huMoments()
    {
        if (!m_done)
        {
            cv::Moments m = cv::moments(m_data);
            cv::HuMoments(m,m_hu);
            for (int i=0; i<7; ++i) if (m_hu[i]<0) m_hu[i]=-log(qAbs(m_hu[i]));
            else                                   m_hu[i]= log(m_hu[i]);
            qWarning(">>>>HU0: %lf", m_hu[0]);

            m_done = true;
        }
        return m_hu;
    }
private:
    std::vector<cv::Point>  m_data;
    bool                    m_done;
    double                  m_hu[7];
};

    class SScCannyContainer
    {
    public:

        SScCannyContainer(const QString& filename, int median = 5, bool eq = true)
        {
            QFileInfo fi(filename);
            qWarning("FILE %s %s", qPrintable(fi.absoluteFilePath()), fi.exists() ? "EXISTS":"DOES NOT EXIST");
            m_mat = cv::imread(filename.toUtf8().constData(),cv::IMREAD_GRAYSCALE);
            qWarning(">>>>>%s %dx%d", filename.toUtf8().constData(), m_mat.cols, m_mat.rows);
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

    class HuNetMainWindow : public QLabel
    {
    public:
        explicit HuNetMainWindow() : QLabel(0)
        {
            setAcceptDrops(true);
            setScaledContents(true);
         setMinimumSize(256,256);
            qWarning("CREATED");
        }
    private:
        void dragEnterEvent(QDragEnterEvent *event)
        {
            event->accept();
        }
        void dragMoveEvent(QDragMoveEvent *event)
        {
            event->accept();
        }
        void dragLeaveEvent(QDragLeaveEvent *event)
        {
            event->accept();
        }
        void dropEvent(QDropEvent* ev)
        {
            const QUrl url(ev->mimeData()->text());
            if (url.isLocalFile())
            {
                const QString filename = url.toLocalFile().trimmed();
                SScCannyContainer cc(filename,21,false);

                auto cont = cc.contours(200,255);

                setPixmap(QPixmap::fromImage(cc.canny()));
            }
        }
    };


int main(int argc, char *argv[])
{

    QApplication app(argc,argv);
    HuNetMainWindow mw;
    mw.show();
    return app.exec();


}
