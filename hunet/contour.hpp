#ifndef CONTOUR_HPP
#define CONTOUR_HPP

#include <QImage>
#include <vector>
#include <opencv2/core.hpp>

class SScContour
{
public:
    SScContour();
    SScContour(const std::vector<cv::Point>& v);
    inline size_t   size    () const { return m_data.size(); }
    inline bool     isEmpty () const { return size()==0; }
    inline bool     isValid () const { return size()>1; }
    QImage draw(int w = 255, double th = 2.0, const QColor& c = Qt::white, bool closed = false) const;
    bool draw(QImage& im, double th = 2.0, const QColor& c = Qt::white, bool closed = false) const;
    bool mark(QImage&, double th = 1.5) const;
    double* huMoments();
    double perimeter() const;
    inline double convexity() const { return isEmpty() ? -1 : convexHull().perimeter()/perimeter(); }
    int diag() const;
    QPair<int,int> xRange() const;
    QPair<int,int> yRange() const;
    bool isSelfIntersected(bool closed=true) const;


    static QStringList labels();
    QStringList values();

    SScContour convexHull() const;
    SScContour hull(double epsion = 1.0) const;
    SScContour approxHull(int nr, int steps = 10000) const;
    SScContour approxConvexHull(int nr, int steps = 10000) const;

private:
    QList<QLineF> lines(bool closed) const;
    SScContour norm(double w) const;
    double minEnclosingCircleRadius () const;
    double convexHullArea           () const;
    double area                     () const;
    double hullArea                 (double epsilon = 2.0) const;

    std::vector<cv::Point>  m_data;
    bool                    m_done;
    double                  m_hu[7];
};

#endif // CONTOUR_HPP
