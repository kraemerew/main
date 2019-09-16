#include "contour.hpp"
#include <opencv2/imgproc.hpp>
#include <QPainter>

SScContour::SScContour() : m_done(false)
{}
SScContour::SScContour(const std::vector<cv::Point>& v) : m_data(v), m_done(false)
{}
QImage SScContour::draw(int w) const
{
    QImage im(w,w,QImage::Format_RGB32);
    im.fill(Qt::black);

    const auto a = norm(w);
    if (a.size()>1)
    {
        QPainter p(&im);
        p.setPen(Qt::white);
        QPoint p1, p2;
        p1 = QPoint(a[0].x, a[0].y);
        for(size_t i=1; i<a.size(); ++i)
        {
            p2 = QPoint(a[i].x, a[i].y);
            p.drawLine(p1,p2);
            p1=p2;
        }
    }
    return im;
}
double* SScContour::huMoments()
{
    if (!m_done)
    {
        cv::Moments m = cv::moments(m_data);
        cv::HuMoments(m,m_hu);
        for (int i=0; i<7; ++i) if (m_hu[i]<0) m_hu[i]=-log(qAbs(m_hu[i]));
        else                                   m_hu[i]= log(m_hu[i]);

        m_done = true;
    }
    return m_hu;
}
std::vector<cv::Point>  SScContour::norm(double w) const
{
    if (m_data.size()==0) return m_data;
    int xmin = m_data.begin()->x, xmax = xmin,
        ymin = m_data.begin()->y, ymax = ymin;
    for(size_t i=0; i<m_data.size(); ++i)
    {
        const int x = m_data[i].x, y = m_data[i].y;
        if (x<xmin) xmin=x; else if (x>xmax) xmax=x;
        if (y<ymin) ymin=y; else if (y>ymax) ymax=y;
    }
    if ((xmax==xmin) || (ymax==ymin)) return m_data;
    const double scx = w/(double)(xmax-xmin),
                 scy = w/(double)(ymax-ymin);

    std::vector<cv::Point> ret;
    ret.reserve(m_data.size());
    for(size_t i=0; i<m_data.size(); ++i)
    {
        cv::Point p;
       p.x = qRound(scx*(double)(m_data[i].x-xmin)),
               p.y = qRound(scy*(double)(m_data[i].y-ymin));
        ret.insert(ret.end(),p);
    }
    return ret;
}
