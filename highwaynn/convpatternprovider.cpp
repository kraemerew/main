#include "convpatternprovider.hpp"
#include "convhelpers.hpp"
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
    /*!
     * \brief Returns the image pixels at a given kernel position
     * \param kx
     * \param ky
     * \param ovl
     * \param xidx  Kernel position index x (this is the output neuron position)
     * \param yidx  Kernel position index y (this is the output neuron position)
     * \return
     */
    QVector<double> pixels(int kx, int ky, int ovl, int xidx, int yidx) const
    {
        return pixels(SSnConvHelpers::convMaskPositions(kx,ky,ovl,xidx,yidx,m_im.width(),m_im.height()));
    }

    /*!
     * \brief Return pixel values
     * \param positions
     * \return
     */
    QVector<double> pixels(const QList<QPoint>& positions) const
    {
        QVector<double> v;
        foreach(const auto& pos, positions)
        {
            const auto px = m_im.pixel(pos);
            if (m_color)
            {
                v << qRed   (px);
                v << qGreen (px);
                v << qBlue  (px);
            }
            else
            {
                v << qGray(m_im.pixel(pos));
            }
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
    auto key = QUuid::createUuid().toString();

    bool success = true;

    const QSize kidxsize = SSnConvHelpers::convMaskFits(m_kx,m_ky,m_ovl,m_xres,m_yres);
    if (kidxsize.isValid())
    {
        qWarning(">>>>> IMAGE %dx%d - KONV %dx%d AT KX %d KY %d OVL %d", m_xres,m_yres,kidxsize.width(),kidxsize.height(),m_kx, m_ky, m_ovl);
        SScConvPattern p(im,m_xres, m_yres, m_isColor);
        m_images[key]=p.m_im;

        for (int y=0; y<kidxsize.height(); ++y) for (int x = 0; x<kidxsize.width(); ++x) if (success)
        {
            const auto px = p.pixels(m_kx,m_ky,m_ovl,x,y);
            if (px.isEmpty()) success = false; else m_patterns[key] << px;
        }
    }
    else success = false;

    if (!success)
    {
        m_images.   remove(key);
        m_patterns. remove(key);
    }
    else
    {
        m_pkeys = m_patterns.keys();
    }
    return success ? key : QString();
}
