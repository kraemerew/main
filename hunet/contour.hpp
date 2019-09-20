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
    QImage draw(int w = 255, double th = 2.0) const;
    bool draw(QImage&, double th = 1.5) const;
    double* huMoments();
    int diag() const;
    QPair<int,int> xRange() const;
    QPair<int,int> yRange() const;

    static QStringList labels();
    QStringList values();

private:
    std::vector<cv::Point>  norm(double w = 255.0) const;
    double minEnclosingCircleRadius () const;
    double convexHullArea           () const;
    double area                     () const;
    double hullArea                 (double epsilon = 2.0) const;

    std::vector<cv::Point>  m_data;
    bool                    m_done;
    double                  m_hu[7];
};

#endif // CONTOUR_HPP
