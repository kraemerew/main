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
    return QStringList() << "Hu1" << "Hu2" << "Hu3" << "Hu4" << "Hu5" << "Hu6" << "Hu7" << "ECRad" << "Convexity";
}
QStringList SScContour::values()
{
    QString s;
    QStringList ret;
    double *dl = huMoments();
    for (int i=0; i<7; ++i) { s.sprintf("%.4lf",dl[i]); ret << s; }
    s.sprintf("%.4lf",minEnclosingCircleRadius()); ret << s;
    s.sprintf("%.4lf",convexity()); ret << s;

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

double SScContour::perimeter() const { return cv::arcLength(m_data,true); }

bool SScContour::mark(QImage& im, double th) const
{
    if (isValid() && !im.isNull())
    {
        QPainter p(&im);
        QPen pen(Qt::red);
        pen.setWidthF(th);
        p.setPen(pen);
        foreach(auto l, lines(false)) p.drawLine(l);
        return true;
    }
    return false;
}

bool SScContour::draw(QImage& im, double th, const QColor& c, bool closed) const
{
    if (isEmpty() || im.isNull()) return false;

    auto ll = norm(qMin(im.width(),im.height())).lines(closed);
    if (!ll.isEmpty())
    {
        QPainter p(&im);
        QPen pen(c);
        pen.setWidthF(th);
        p.setPen(pen);
        foreach(auto l, ll) p.drawLine(l);
    }
    return true;
}

QImage SScContour::draw(int w, double th, const QColor& c, bool closed) const
{
    QImage im(w,w,QImage::Format_RGB32);
    im.fill(Qt::black);
    draw(im,th,c,closed);
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

SScContour SScContour::norm(double w) const
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
    return SScContour(ret);
}



double SScContour::area() const
{
    return cv::contourArea(m_data);
}
double SScContour::convexHullArea() const
{
    return convexHull().area();
}
double SScContour::hullArea(double epsilon) const
{
    return hull(epsilon).area();
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

SScContour SScContour::hull(double epsilon) const
{
    std::vector<cv::Point> data;
    cv::approxPolyDP(m_data,data,epsilon,true);
    return SScContour(data);
}

SScContour SScContour::convexHull() const
{
    std::vector<cv::Point> data;
    cv::convexHull(m_data,data);
    return SScContour(data);
}

SScContour SScContour::approxHull(int nr, int steps) const
{
    Q_ASSERT(nr>2);
    if (nr>2)
    {
        double p = perimeter();
        for (int i=1; i<=steps; ++i)
        {
            const double epsilon = (p*(double)i)/(double)steps;
            auto c = hull(epsilon);
            if (c.size()<=(size_t)nr) return c;
        }
    }
    return SScContour();
}

SScContour SScContour::approxConvexHull(int nr, int steps) const
{
    Q_ASSERT(nr>2);
    if (nr>2)
    {
        double p = perimeter();
        for (int i=1; i<=steps; ++i)
        {
            const double epsilon = (p*(double)i)/(double)steps;
            auto c = hull(epsilon).convexHull();
            if (c.size()<=(size_t)nr) return c;
        }
    }
    return SScContour();
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

QList<QLineF> SScContour::lines(bool closed) const
{
    QList<QLineF> ret;
    if (isEmpty()) return ret;
    QPoint p1 = QPoint(m_data[0].x, m_data[0].y), p2, pfirst = p1;


    for(size_t i=1; i<size(); ++i)
    {
        p2 = QPoint(m_data[i].x, m_data[i].y);
        ret << QLineF(p1,p2);
        p1 = p2;
    }
    if (closed) ret << QLineF(pfirst,p1);
    return ret;
}

bool SScContour::isSelfIntersected(bool closed) const
{
    if (isEmpty()) return false;
    QList<QLineF> ll = lines(closed), donell;

    foreach(auto l, ll)
    {
        foreach(auto other, donell)
        {
            QPointF p;
            if (l.intersect(other,&p) == QLineF::BoundedIntersection) return true;
        }
        donell << l;
    }
    return false;
}
