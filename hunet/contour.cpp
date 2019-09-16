#include "contour.hpp"
#include <opencv2/imgproc.hpp>
#include <QPainter>
#include <QtMath>

SScContour::SScContour() : m_done(false)
{}
SScContour::SScContour(const std::vector<cv::Point>& v) : m_data(v), m_done(false)
{}

QString SScContour::label()
{
    QStringList sl;
    double* h = huMoments();
    for (int i=0; i<7; ++i)
    {
        QString s;
        s.sprintf("%.2lf",h[i]);
        sl << s;
    }
    return sl.join(" ");
}
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
    const auto xr = xRange(), yr = yRange();
    if ((xr.first==xr.second) || (yr.first==yr.second)) return m_data;
    const double scx = w/(double)(xr.second-xr.first),
                 scy = w/(double)(yr.second-yr.first);

    std::vector<cv::Point> ret;
    ret.reserve(m_data.size());
    for(size_t i=0; i<m_data.size(); ++i)
    {
        cv::Point p;
        p.x = qRound(scx*(double)(m_data[i].x-xr.first)),
        p.y = qRound(scy*(double)(m_data[i].y-yr.first));
        ret.insert(ret.end(),p);
    }
    return ret;
}

int SScContour::diag() const
{
    const auto xr = xRange(), yr = yRange();
    return qRound(qSqrt(qPow(xr.second-xr.first,2.0)+qPow(yr.second-yr.first,2.0)));
}

QPair<int,int> SScContour::xRange() const
{
    QPair<int,int> ret(0,0);
    if (size()==0) return ret;
    ret.first = m_data[0].x;
    ret.second = ret.first;
    for(size_t i=0; i<m_data.size(); ++i)
    {
        const int x = m_data[i].x;
        if (x<ret.first)  ret.first=x; else
        if (x>ret.second) ret.second=x;
    }
    return ret;
}


QPair<int,int> SScContour::yRange() const
{
    QPair<int,int> ret(0,0);
    if (size()==0) return ret;
    ret.first = m_data[0].y;
    ret.second = ret.first;
    for(size_t i=0; i<m_data.size(); ++i)
    {
        const int y = m_data[i].y;
        if (y<ret.first)  ret.first=y; else
        if (y>ret.second) ret.second=y;
    }
    return ret;
}
