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
    m_valid = m_valid && (m_kx>0) && (m_ky>0) && (m_ovl>=0) && (m_outw>0) && (m_outh>0);
    if (m_valid)
    {
        m_im = m_im.scaled(inputWidth(),inputHeight(), Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        qWarning(">>>>>>>>>SCALING TO %d x %d", m_im.width(), m_im.height());
    }
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
    m_valid = m_valid && (m_kx>0) && (m_ky>0) && (m_ovl>=0) && (m_outw>0) && (m_outh>0);
    if (m_valid)
    {
        m_im = m_im.scaled(inputWidth(),inputHeight(), Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}

QVector<double> SScConvImageConverter::pattern(int x, int y) const
{
    QVector<double> ret;
    ret.reserve(dim());
    if ((x>=0) && (x<m_outw) && (y>=0) && (y<m_outh))
    {
        for (int j=topLeftY(y); j<topLeftY(y)+m_ky; ++j) for (int i = topLeftX(x); i<topLeftX(x)+m_kx; ++i)
            qWarning("> PIXEL %d %d", i, j);

        if (m_color) for (int j=topLeftY(y); j<topLeftY(y)+m_ky; ++j) for (int i = topLeftX(x); i<topLeftX(x)+m_kx; ++i)
        {
            const auto px = m_im.pixel(i,j);
            ret << qRed     (px);
            ret << qGreen   (px);
            ret << qBlue    (px);

        }
        else for (int j=topLeftY(y); j<topLeftY(y)+m_ky; ++j) for (int i = topLeftX(x); i<topLeftX(x)+m_kx; ++i)
        {
            const auto px = m_im.pixel(i,j);
            ret << qGray    (px);
        }
        Q_ASSERT(ret.size()==dim());
    }
    return ret;
}
