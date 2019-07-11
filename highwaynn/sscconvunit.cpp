#include "sscconvunit.hpp"
#include "../blas/blasvector.hpp"
#include <QUuid>

class SScConvPattern
{
public:
    explicit SScConvPattern(const QImage& im) : m_im(im)
    {
        (void) m_im.convertToFormat(QImage::Format_RGB888);
    }

    inline bool convert(int w, int h)
    {
        if (!isValid()) return false;
        m_im.scaled(w,h,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        return isValid();
    }
    inline bool isValid() const { return !m_im.isNull(); }
    QVector<double> vect(int topleftx, int toplefty, int w, int h, bool color = true) const
    {
        QVector<double> v;
        Q_ASSERT(topleftx+w-1<m_im.width ());
        Q_ASSERT(toplefty+h-1<m_im.height());
        if (color)
        {
            v.reserve(w*h*3);
            for (int y=toplefty; y<toplefty+h-1; ++y) for (int x=topleftx; x<topleftx+w-1; ++x)
            {
                const auto px = m_im.pixel(x,y);
                v << qRed(px);
                v << qGreen(px);
                v << qBlue(px);
            }
        }
        else
        {
            v.reserve(w*h);
            for (int y=toplefty; y<toplefty+h-1; ++y) for (int x=topleftx; x<topleftx+w-1; ++x)
                v << qGray(m_im.pixel(x,y));
        }
        return v;
    }
    QImage m_im;
};


SScConvUnit::SScConvUnit(int kx, int ky, int unitsx, int unitsy, int overlap, int pooling, bool color) :
    m_kx        (kx),
    m_ky        (ky),
    m_unitsx    (unitsx),
    m_unitsy    (unitsy),
    m_ovl       (overlap),
    m_pooling   (pooling),
    m_color     (color)
{
    if (m_pooling<1) m_pooling=1;

    m_kx=qMax(3,m_kx);
    m_ky=qMax(3,m_ky);

    if (m_kx%2==0) ++m_kx;
    if (m_ky%2==0) ++m_ky;
    if (pooling>1)
    {
        while (m_unitsx%m_pooling!=0) ++m_unitsx;
        while (m_unitsy%m_pooling!=0) ++m_unitsy;
    }
    for (int i=0; i<m_kx*m_ky; ++i) m_w << (double)qrand()/(double)RAND_MAX; //<< TODO - reset
}

QString SScConvUnit::addPattern(const QImage &im)
{
    bool success = true;
    SScConvPattern p(im);
    p.convert(xpixels(),ypixels());
    auto key = QUuid::createUuid().toString();
    m_images[key]=p.m_im;
    for (int y=0; y<m_unitsy; ++y) for (int x = 0; x<m_unitsx; ++x)
    {
        const int topleftx = x*m_kx-x*m_ovl, toplefty = y*m_ky-y*m_ovl;
        auto v = p.vect(topleftx, toplefty, m_kx, m_ky, m_color);
        if (v.isEmpty()) success = false; else m_patterns[key] << v;
    }
    if (!success)
    {
        m_images.remove(key);
        m_patterns.remove(key);
    }
    else m_pkeys = m_patterns.keys();
    return success ? key : QString();
}

QString SScConvUnit::nextPattern()
{
    if (!m_pkeys.isEmpty())
    {
        const QString key = m_pkeys.takeFirst();
        m_pkeys << key;
        if (activatePattern(key)) return key;
    }
    return QString();
}
bool SScConvUnit::activatePattern(const QString& uuid)
{
    if (m_patterns.contains(uuid) && (m_patterns[uuid].size()==units()))
    {
        m_n.clear();
        m_n.reserve(units());

        foreach(const QVector<double>& dv, m_patterns[uuid])
        {
            m_n << SSnBlas::dot(m_w,dv);
        }
        return true;
    }
    return false;
}
