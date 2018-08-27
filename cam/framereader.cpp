#include "framereader.hpp"
#include <QByteArray>



class SScFrameReaderMJPG : public SScFrameReaderBase
{
public:
    SScFrameReaderMJPG(quint32 w, quint32 h) : SScFrameReaderBase(w,h) {}
    virtual QImage get(void* ptr, quint32 len) const
    {
        QImage im;
        if (im.loadFromData(QByteArray::fromRawData((const char*)ptr, len)))
        {
            return im;
        }
        return QImage(m_w, m_h, QImage::Format_RGB888);
    }
};


class SScFrameReaderYUYV : public SScFrameReaderBase
{
public:
    SScFrameReaderYUYV(quint32 w, quint32 h) : SScFrameReaderBase(w,h) {}
    virtual QImage get(void* ptr, quint32 len) const
    {
        QImage im(m_w,m_h,QImage::Format_RGB888);
        quint32* data = (quint32*)ptr;
        quint32 x = 0, y = 0;
        Q_ASSERT(len/2==m_w*m_h);
        for (quint32 i=0; i<len; ++i) if (y<m_h)
        {
            quint32 yuyv = data[i];
            uchar y1 = yuyv&0xff;
            yuyv>>=8;
            uchar u = yuyv&0xff;
            yuyv>>=8;
            uchar y2 = yuyv&0xff;
            yuyv >>=8;
            uchar v = yuyv&0xff;
            im.setPixel(x,y,yuv2rgb(y1,u,v));

            if (++x==m_w) { x=0; ++y; }
            im.setPixel(x,y,yuv2rgb(y2,u,v));
            if (++x==m_w) { x=0; ++y; }
        }
        if (im.loadFromData(QByteArray::fromRawData((const char*)ptr, len)))
        {
            return im;
        }
        return QImage(m_w, m_h, QImage::Format_RGB888);
    }
private:
    QRgb yuv2rgb(int y, int u, int v) const
    {
       int r1, g1, b1;
       int c = y-16, d = u - 128, e = v - 128;

       r1 = (298 * c           + 409 * e + 128) >> 8;
       g1 = (298 * c - 100 * d - 208 * e + 128) >> 8;
       b1 = (298 * c + 516 * d           + 128) >> 8;

       // Even with proper conversion, some values still need clipping.

       if (r1 > 255) r1 = 255;
       if (g1 > 255) g1 = 255;
       if (b1 > 255) b1 = 255;
       if (r1 < 0) r1 = 0;
       if (g1 < 0) g1 = 0;
       if (b1 < 0) b1 = 0;

       return qRgb(r1,g1,b1);
    }
};



SScFrameReaderBase* SScFrameReaderBase::create(const QString& fourcc, quint32 w, quint32 h)
{
    if (fourcc=="MJPG") return new SScFrameReaderMJPG(w,h); else
    if (fourcc=="YUYV") return new SScFrameReaderYUYV(w,h);
    return NULL;
}
