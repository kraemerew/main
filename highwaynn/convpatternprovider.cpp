#include "convpatternprovider.hpp"
#include <QUuid>

class SScConvPattern
{
public:
    explicit SScConvPattern(const QImage& im, int w, int h, bool color) : m_im(im), m_w(w), m_h(h), m_color(color)
    {
        Q_ASSERT(m_w*m_h>0);
        if (color) (void) m_im.convertToFormat(QImage::Format_RGB888);
        else       (void) m_im.convertToFormat(QImage::Format_Grayscale8);
        m_im = m_im.scaled(w,h,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }

    inline bool isValid() const { return !m_im.isNull(); }
    inline int vSize() const { return m_color ? m_w*m_h*3 : m_w*m_h; }
    QVector<double> vect(int topleftx, int toplefty, int w, int h) const
    {
        QVector<double> v;
        v.reserve(m_color ? w*h*3 : w*h);
        Q_ASSERT(topleftx+w-1<m_im.width ());
        Q_ASSERT(toplefty+h-1<m_im.height());
        if (m_color)
        {
            for (int y=toplefty; y<toplefty+h; ++y) for (int x=topleftx; x<topleftx+w; ++x)
            {
                const auto px = m_im.pixel(x,y);
                v << qRed(px);
                v << qGreen(px);
                v << qBlue(px);
            }
        }
        else
        {
            for (int y=toplefty; y<toplefty+h; ++y) for (int x=topleftx; x<topleftx+w; ++x)
                v << qGray(m_im.pixel(x,y));
        }
        return v;
    }
    QImage m_im;
    int m_w, m_h;
    bool m_color;
};


void SScConvPatternProvider::reconfigure(int x, int y, bool c, int kx, int ky, int ovl)
{
    Q_ASSERT(x>0);
    Q_ASSERT(y>0);
    Q_ASSERT(kx>0);
    Q_ASSERT(kx>0);
    Q_ASSERT(ovl>=0);
    m_xres=x;
    m_yres=y;
    m_kx= kx;
    m_ky=ky;
    m_ovl=ovl;
    m_isColor = c;
}

QString SScConvPatternProvider::addPattern(const QString& filename)
{
    QImage im;
    im.load(filename);
    return !im.isNull() ? addPattern(im) : QString();
}
QString SScConvPatternProvider::addPattern(const QImage &im)
{
    bool success = true;
    SScConvPattern p(im,m_xres, m_yres, m_isColor);
    auto key = QUuid::createUuid().toString();
    m_images[key]=p.m_im;
    qWarning(">>ADDED %dx%d %dbpp", p.m_im.width(), p.m_im.height(), p.m_im.depth());
    for (int y=0; y<m_yres; ++y) for (int x = 0; x<m_xres; ++x)
    {
        const int topleftx = x*m_kx-x*m_ovl, toplefty = y*m_ky-y*m_ovl;
        auto v = p.vect(topleftx, toplefty, m_kx, m_ky);
        qWarning(">>>>>>>>>VECTOR %d,%d -> %d", topleftx,toplefty,v.size());
        if (v.isEmpty()) success = false; else m_patterns[key] << v;
    }
    if (!success)
    {
        qWarning(">>>PATTERN NOT ADDED");
        m_images.remove(key);
        m_patterns.remove(key);
    }
    else
    {
        m_pkeys = m_patterns.keys();
    }
    return success ? key : QString();
}
