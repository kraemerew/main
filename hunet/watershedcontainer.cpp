#include "watershedcontainer.hpp"
#include "watershed.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QtMath>

SScWatershedContainer::SScWatershedContainer() : m_valid(false) {}

SScWatershedContainer::SScWatershedContainer(const QString& filename, const SScVM& setting) : m_valid(false)
{
    m_orig = cv::imread(filename.toUtf8().constData(),cv::IMREAD_COLOR);
    m_valid = (m_orig.cols>0) && (m_orig.rows>0);
    if (m_valid)
    {
        m_contours = SSnWatershed::execute(m_orig,SSnWatershed::Pars(setting), m_bw, m_dist, m_markers);
        m_dist.convertTo(m_dist,CV_8UC1);
        m_markers.convertTo(m_markers,CV_8UC3);
    }
}
QList<SScContour> SScWatershedContainer::contours()
{
    return m_contours;
}

QStringList SScWatershedContainer::images() { return QStringList() << "Orig" << "Bin" << "Dist" << "Markers"; }

QImage SScWatershedContainer::image(int idx) const
{
    switch(idx)
    {
        case 0: return SSnImageConverter::image(m_orig); break;
        case 1: return SSnImageConverter::image(m_bw); break;
        case 2: return SSnImageConverter::image(m_dist); break;
        case 3: return SSnImageConverter::image(m_markers); break;
    }
    return QImage();
}
