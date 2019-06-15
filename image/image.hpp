#ifndef SSCIMAGE_HPP
#define SSCIMAGE_HPP

#include "matrix.hpp"
#include <QImage>
class SScImage : public QImage
{

public:
    SScImage() : QImage() {}
    SScImage(const QImage& im) : QImage(converted(im))
    {}
    SScImage(const SScUCMatrix& m) : QImage(m.width(),m.height(),QImage::Format_Grayscale8)
    {
        for (quint32 i=0; i<m.height(); ++i)
        {
            const uchar *l = m.constLine(i);
            memcpy(scanLine(i),l,m.width());
        }
    }

    SScImage(const SScUCMatrix& r, const SScUCMatrix& g, const SScUCMatrix& b)
        : QImage(r.width(),r.height(),QImage::Format_RGB888)
    {
        if ((r.size()==g.size()) && (g.size()==b.size()))
        {
            for (quint32 i=0; i<r.height(); ++i)
            {
                const uchar *rl = r.constLine(i), *gl = g.constLine(i), *bl=b.constLine(i);                
                for (quint32 j=0; j<r.width(); ++j) setPixel(j,i,qRgb(*rl++,*gl++,*bl++));
            }
        }
    }

    SScUCMatrix grey()
    {
        if (m_m.isEmpty()) toMono(); else qWarning("MATRIX ALREADY DONE");
        return m_m;
    }
    SScUCMatrix red    ()
    {
        if (m_r.isEmpty()) toRGB(); else qWarning("MATRIX ALREADY DONE");
        return m_r;
    }
    SScUCMatrix green    ()
    {
        if (m_g.isEmpty()) toRGB(); else qWarning("MATRIX ALREADY DONE");
        return m_g;
    }
    SScUCMatrix blue   ()
    {
        if (m_b.isEmpty()) toRGB(); else qWarning("MATRIX ALREADY DONE");
        return m_b;
    }

private:
    SScUCMatrix m_r, m_g, m_b, m_m;
    inline QImage converted(const QImage& im) const
    {
        if (im.allGray()) return im.convertToFormat(QImage::Format_Grayscale8);
        if (im.format()!=QImage::Format_RGB888) return im.convertToFormat(QImage::Format_RGB888);
        return im;
    }

    inline void toMono()
    {
        m_m = SScUCMatrix(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* m = m_m.line(i);
            for (int j=0; j<width(); ++j)
            {
                QRgb c = pixel(j,i);
                (*m++)=qGray (c);
            }
        }
    }

    inline void toRGB()
    {
        m_r = SScUCMatrix(width(),height());
        m_g = SScUCMatrix(width(),height());
        m_b = SScUCMatrix(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* r = m_r.line(i), *b = m_b.line(i), *g = m_g.line(i);
            for (int j=0; j<width(); ++j)
            {
                QRgb c = pixel(j,i);
                (*r++)=qRed  (c);
                (*g++)=qGreen(c);
                (*b++)=qBlue (c);
             }
        }
        //qWarning("Converted %d %d %d %d %d %d", m_r.width(),m_r.height(),m_g.width(),m_g.height(),m_b.width(),m_b.height());
    }
};

#endif // IMAGE_HPP
