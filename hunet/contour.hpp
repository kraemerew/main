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
    inline size_t size() const { return m_data.size(); }
    QImage draw(int w = 255) const;
    double* huMoments();
    QString label();
    int diag() const;
    QPair<int,int> xRange() const;
    QPair<int,int> yRange() const;

private:
    std::vector<cv::Point>  norm(double w = 255.0) const;

    std::vector<cv::Point>  m_data;
    bool                    m_done;
    double                  m_hu[7];
};

#endif // CONTOUR_HPP
