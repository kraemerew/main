#include "contour.hpp"
#include <opencv2/imgproc.hpp>
#include <QPainter>
#include <QtMath>

SScContour::SScContour() : m_done(false)
{}
SScContour::SScContour(const std::vector<cv::Point>& v) : m_data(v), m_done(false)
{}

QStringList SScContour::labels()
{
    return QStringList() << "Hu1" << "Hu2" << "Hu3" << "Hu4" << "Hu5" << "Hu6" << "Hu7" << "ECRad";
}
QStringList SScContour::values()
{
    QString s;
    QStringList ret;
    double *dl = huMoments();
    for (int i=0; i<7; ++i) { s.sprintf("%.4lf",dl[i]); ret << s; }
    s.sprintf("%.4lf",minEnclosingCircleRadius()); ret << s;
    return ret;
}
/*
QString SScContour::label()
{

    const double  r = minEnclosingCircleRadius(), a = area(), ha = hullArea(), cva = convexHullArea();
    if (a>0)
    {
        QString s;
        s.sprintf("Area %.2lf",a);
        sl << s;
    }
    if (ha>0)
    {
        QString s;
        s.sprintf("HullArea %.2lf",ha);
        sl << s;
    }
  if (r>0)
    {
       const double ca = (r*r*3.14159265);
        QString s;
        s.sprintf("CircleArea %.2lf",ca);
        sl << s;
    }
    if (cva>0)
    {
        QString s;
        s.sprintf("ConvHullArea %.2lf",cva);
        sl << s;
    }

    return sl.join(" ");
}*/

bool SScContour::draw(QImage& im, double th) const
{
    if (isValid() && !im.isNull())
    {
        QPainter p(&im);
        QPen pen(Qt::red);
        pen.setWidthF(th);
        p.setPen(pen);
        QPoint p1, p2;
        p1 = QPoint(m_data[0].x, m_data[0].y);
        for(size_t i=1; i<size(); ++i)
        {
            p2 = QPoint(m_data[i].x, m_data[i].y);
            p.drawLine(p1,p2);
            p1=p2;
        }
        return true;
    }
    return false;
}
QImage SScContour::draw(int w, double th) const
{
    QImage im(w,w,QImage::Format_RGB32);
    im.fill(Qt::black);

    const auto a = norm(w);
    if (a.size()>1)
    {
        QPainter p(&im);
        QPen pen(Qt::white);
        pen.setWidthF(th);
        p.setPen(pen);
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
    const int width = xr.second-xr.first, height = yr.second-yr.first,
              refsc = (width>height) ? width : height;
    if ((xr.first==xr.second) || (yr.first==yr.second)) return m_data;

    const double scx = w/(double)(refsc),
                 scy = w/(double)(refsc);

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

double SScContour::area() const
{
    return cv::contourArea(m_data);
}
double SScContour::convexHullArea() const
{
    std::vector<cv::Point> data;
    cv::convexHull(m_data,data);
    return cv::contourArea(data);
}
double SScContour::hullArea(double epsilon) const
{
    std::vector<cv::Point> data;
    cv::approxPolyDP(m_data,data,epsilon,true);
    return cv::contourArea(data);
}
double SScContour::minEnclosingCircleRadius() const
{
    cv::Point2f center;
    float radius = 0.0;
    cv::minEnclosingCircle(m_data,center,radius);
    return radius;
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

