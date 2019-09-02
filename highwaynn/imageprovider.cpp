#include "imageprovider.hpp"
#include "image/convimageconverter.hpp"
#include <QUuid>

SScImageProvider::SScImageProvider(int kx, int ky, int overlap, int outw, int outh, bool color)
  : m_kx    (kx),
    m_ky    (ky),
    m_ovl   (overlap),
    m_outw  (outw),
    m_outh  (outh),
    m_color (color)
{}


QString SScImageProvider::addPattern(const QString &filename)
{
    QImage im;
    if (im.load(filename)) return addPattern(im);
    return QString();
}

QString SScImageProvider::addPattern(const QImage &im)
{
    SScConvImageConverter cic(m_kx,m_ky,m_ovl,m_outw,m_outh,m_color,im);
    if (cic.isValid())
    {
        const auto key = QUuid::createUuid().toString();
        m_patterns[key]=cic.matrix();
        return key;
    }
    return QString();
}

