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
    SScImage(const SScMatrix<uchar>& m) : QImage(m.width(),m.height(),QImage::Format_Grayscale8)
    {
    }

    SScImage(const SScMatrix<uchar>& r, const SScMatrix<uchar>& g, const SScMatrix<uchar>& b)
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

    SScMatrix<uchar> grey()
    {
        if (!m_m.isEmpty() || format()!=QImage::Format_Grayscale8) return m_m;

        m_m = SScMatrix<uchar>(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* l = m_m.line(i);
            for (int j=0; j<width(); ++j)
            {
                (*l)=qGray(pixel(j,i));
                l++;
            }
        }
        return m_r;
    }


    SScMatrix<uchar> red    ()
    {
        if (!m_r.isEmpty()) return m_r;

        m_r = SScMatrix<uchar>(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* l = m_r.line(i);
            for (int j=0; j<width(); ++j)
            {
                (*l)=qRed(pixel(j,i));
                l++;
            }
        }
        return m_r;
    }
    SScMatrix<uchar> green    ()
    {
        if (!m_g.isEmpty()) return m_g;
        m_g = SScMatrix<uchar>(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* l = m_g.line(i);
            for (int j=0; j<width(); ++j)
            {
                (*l)=qGreen(pixel(j,i));
                l++;
            }
        }
        return m_g;
    }
    SScMatrix<uchar> blue    ()
    {
        if (!m_b.isEmpty()) return m_b;
        m_b = SScMatrix<uchar>(width(),height());
        for (int i=0; i<height(); ++i)
        {
            uchar* l = m_b.line(i);
            for (int j=0; j<width(); ++j)
            {
                (*l)=qBlue(pixel(j,i));
                l++;
            }
        }
        return m_b;
    }




private:
    SScMatrix<uchar> m_r, m_g, m_b, m_m;
    inline QImage converted(const QImage& im) const
    {
        if (im.allGray()) return im.convertToFormat(QImage::Format_Grayscale8);
        if (im.format()!=QImage::Format_RGB888) return im.convertToFormat(QImage::Format_RGB888);
        return im;
    }
};

#endif // IMAGE_HPP
