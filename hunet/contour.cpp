#include "contour.hpp"
#include "sscvm.hpp"
#include <opencv2/imgproc.hpp>
#include <QPainter>
#include <QDataStream>
#include <QtMath>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>

SScContour::SScContour() : m_done(false)
{}

SScContour::SScContour(const std::vector<cv::Point>& v) : m_data(v), m_done(false)
{}

SScContour::SScContour(const QVariantMap &data)
{
    SScVM vm(data);
    QVariantList xl = vm.vlToken("X"),
                 yl = vm.vlToken("Y");
    if (xl.size()==yl.size())
    {
        m_tag = vm.stringToken("TAG");
        m_data.reserve(xl.size());
        for (int i=0; i<xl.size(); ++i) if (xl[i].canConvert<int>() && yl[i].canConvert<int>())
        {
            cv::Point p;
            p.x = xl[i].toInt();
            p.y = yl[i].toInt();

            m_data.insert(m_data.end(), p);
        }
    }
}

QVariantMap SScContour::vm() const
{
    QVariantMap ret;
    QVariantList xl, yl;
    xl.reserve(m_data.size());
    yl.reserve(m_data.size());
    for (size_t i = 0; i<m_data.size(); ++i) { xl << m_data[i].x; yl << m_data[i].y; }
    ret["X"] = xl;
    ret["Y"] = yl;
    ret["TAG"] = m_tag;
    return ret;
}

QStringList SScContour::contourLabels()
{
    QStringList ret;
    ret << "ConvexHull";
    ret << "FitEllipse";
    for (int i=3; i<15; ++i) ret << QString("Convex_Approx_%1").arg(i);
    return ret;
}

SScContour SScContour::contour(const QString& id) const
{
    SScContour ret;
    if (id.startsWith("FitEllipse"))
    {
        ret = fitEllipse();
    }
    else if (id.startsWith("Convex_Approx_"))
    {
        bool ok = false;
        const int edges = id.right(id.length()-14).toInt(&ok);
        if (ok) ret = approxConvexHull(edges);
    }
    else
    if (id=="ConvexHull")
    {
        ret = convexHull();
    }
    return ret;
}

QStringList SScContour::featureLabels()
{
    return QStringList() << "Hu1" << "Hu2" << "Hu3" << "Hu4" << "Hu5" << "Hu6" << "Hu7" << "Convexity" << "Circularity";
}
QStringList SScContour::featureValues()
{
    QString s;
    QStringList ret;
    double *dl = huMoments();
    for (int i=0; i<7; ++i) { s.sprintf("%.4lf",dl[i]); ret << s; }
    s.sprintf("%.4lf",convexity()); ret << s;
    s.sprintf("%.4lf",circularity()); ret << s;
    return ret;
}

double SScContour::circularity() const
{
    cv::RotatedRect r = cv::fitEllipse(m_data);
    const auto w = r.size.width, h = r.size.height;
    if ((w==0) || (h==0)) return -1;
    return w>h ? h/w : w/h;
}

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
        for (int i=0; i<7; ++i)
        {
            const double sign = (m_hu[i]<0) ? -1 : 1;
            m_hu[i]=-sign*log(qAbs(m_hu[i]));
        }
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

SScContour SScContour::fitEllipse() const
{
    cv::RotatedRect rr = cv::fitEllipse(m_data);
    cv::Point2f pts[4];
    rr.points(pts);
    std::vector<cv::Point> pv;
    for (int i=0; i<4; ++i)
    {
        cv::Point p;
        p.x= qRound(pts[i].x);
        p.y= qRound(pts[i].y);
        pv.insert(pv.end(),p);
    }
    pv.insert(pv.end(),*pv.begin());
    return SScContour(pv);
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

QString SScContour::md5()
{

    if (m_md5.isEmpty())
    {
        QByteArray data;
        QDataStream ds(&data,QIODevice::WriteOnly);
        for (size_t i=0; i<m_data.size(); ++i)
        {
            ds << m_data[i].x;
            ds << m_data[i].y;
        }
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(data);
        m_md5=hash.result().toHex();
    }
    return m_md5;
}

QVariantMap SScContour::toVM(const QList<SScContour> &cl)
{
    QVariantMap ret;
    foreach(auto c, cl) ret[c.md5()] = c.vm();
    return ret;
}

QString SScContour::toJson(const QList<SScContour>& cl)
{
    QJsonDocument jd;
    jd.setObject(QJsonObject::fromVariantMap(toVM(cl)));
    return jd.toJson();
}

QList<SScContour> SScContour::fromJson(const QString &data)
{
    QList<SScContour> ret;
    QJsonDocument jd = QJsonDocument::fromJson(data.toUtf8());
    foreach(auto v, jd.object().toVariantMap()) if (v.type()==QVariant::Map)
    {
        SScContour c(v.toMap());
        ret << c;
    }
    return ret;
}

QString SScContour::tag     () const                { return m_tag; }
void    SScContour::setTag  (const QString &newtag) { m_tag=newtag; }




SScContourSet::SScContourSet(const QList<SScContour>& cl)
{
    foreach(SScContour c, cl) add(c);
}
SScContourSet::SScContourSet(const QString &jsondata)
{
    fromJson(jsondata);
}
bool SScContourSet::add(const SScContour& c)
{
    const QString md5 = SScContour(c).md5();
    if (contains(md5)) return false;
    (*this)[md5]=c;
    return true;
}
void    SScContourSet::add      (const SScContourSet& other)        { add(other.toList()); }
void    SScContourSet::add      (const QList<SScContour>& cl)       { foreach(const auto& c, cl) add(c); }
QString SScContourSet::toJSon   () const                            { return SScContour::toJson(values()); }
void    SScContourSet::fromJson (const QString &data, bool merge)   { if (!merge) clear(); add(SScContour::fromJson(data)); }
