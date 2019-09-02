#include "convimageconverter.hpp"

SScConvImageConverter::SScConvImageConverter(int kx, int ky, int overlap, int outw, int outh, bool color, const QString& file)
    : m_im      (QImage()),
      m_kx      (kx),
      m_ky      (ky),
      m_ovl     (overlap),
      m_outw    (outw),
      m_outh    (outh),
      m_color   (color),
      m_valid   (m_im.load(file))
{
    init();
}

SScConvImageConverter::SScConvImageConverter(int kx, int ky, int overlap, int outw, int outh, bool color, const QImage& im)
    : m_im      (im),
      m_kx      (kx),
      m_ky      (ky),
      m_ovl     (overlap),
      m_outw    (outw),
      m_outh    (outh),
      m_color   (color),
      m_valid   (!m_im.isNull())
{
    init();
}

void SScConvImageConverter::init()
{
    m_valid = m_valid && (m_kx>0) && (m_ky>0) && (m_ovl>=0) && (m_outw>0) && (m_outh>0);
    if (m_valid)
    {
        m_im = m_im.scaled(inputWidth(),inputHeight(), Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        if (m_color) m_im = m_im.convertToFormat(QImage::Format_RGB888);
        else         m_im = m_im.convertToFormat(QImage::Format_Grayscale8);
    }
}

QVector<double> SScConvImageConverter::pattern(int x, int y) const
{
    QVector<double> ret;
    const auto im = cut(x,y);
    if (!im.isNull())
    {
        ret.reserve(dim());
        if (!m_color) for (int j=0; j<m_ky; ++j) for (int i=0; i<m_kx; ++i)
        {
            ret << qGray(im.pixel(i,j));
        }
        else for (int j=0; j<m_ky; ++j) for (int i=0; i<m_kx; ++i)
        {
            const auto px = m_im.pixel(i,j);
            ret << qRed     (px);
            ret << qGreen   (px);
            ret << qBlue    (px);
        }
        Q_ASSERT(ret.size()==dim());
    }
    return ret;
}

QVector<double> SScConvImageConverter::matrix() const
{
    QVector<double> ret;
    ret.reserve(dim()*m_outw*m_outh);
    for (int j=0; j<m_outh; ++j) for (int i=0; i<m_outw; ++i) ret << pattern(i,j);
    return ret;
}

QImage SScConvImageConverter::cut(int x, int y) const
{
    QImage ret;
    if (isValid() && (x>=0) && (x<m_outw) && (y>=0) && (y<m_outh))
        ret = m_im.copy(QRect(topLeft(x,y), kernelSize()));
    return ret;
}
