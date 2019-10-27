#ifndef WATERSHETCONTAINER_HPP
#define WATERSHETCONTAINER_HPP


#include <QImage>
#include <opencv2/core.hpp>
#include "contour.hpp"
#include "imageconverter.h"
#include "sscvm.hpp"

class SScWatershedContainer
{
public:
    SScWatershedContainer();
    SScWatershedContainer(const QString& filename, const SScVM& setting);
    QList<SScContour> contours();

            QImage      image   (int idx=0) const;
    static  QStringList images  ();
    inline  bool        isValid () const { return m_valid; }

private:
    bool m_valid;
    cv::Mat m_orig, m_bw, m_dist, m_markers;
    QList<SScContour> m_contours;
};

#endif // WATERSHETCONTAINER_HPP
