#include "convhelpers.hpp"

SScConvSetting::SScConvSetting(const QSize &kernel, const QSize &stride, const QSize &io, bool isInput)
    : m_k(kernel),
      m_s(stride)
{
    if (kernelValid() && strideValid() && (io.width()>1) && (io.height()>0))
    {
        if (isInput)
        {
            m_i = io;
            m_o = SSnConvHelper::fitToInput(io,m_k,m_s);
        }
        else
        {
            m_o = io;
            m_i = SSnConvHelper::inputSize(m_k,m_s,m_o);
        }
    }
}

SScConvSetting::SScConvSetting(const QSize &kernel, const QSize &stride, const SScConvSetting &input)

    : m_k(kernel),
      m_s(stride),
      m_o(QSize(-1,-1)),
      m_i(input.input())
{
    if (kernelValid() && strideValid() && inputValid())
        m_o = SSnConvHelper::fitToInput(m_i,m_k,m_s);
}

QList<QPoint> SSnConvHelper::convPositions(const QSize &kernel, const QSize &stride, const QPoint &element)
{
    if (!isSane(kernel,stride)) return QList<QPoint>();
    QList<QPoint> ret;
    ret.reserve(kernel.width()*kernel.height());
    const auto topl = topLeftPosition(stride,element);
    for (int j=0; j<kernel.height(); ++j) for (int i=0; i<kernel.width(); ++i)
        ret << topl+QPoint(i,j);
    return ret;
}
QPoint SSnConvHelper::topLeftPosition(const QSize &stride, const QPoint &element)
{
    const int x = stride.width()*element.x(),
              y = stride.height()*element.y();
    return QPoint(x,y);
}
QSize SSnConvHelper::inputSize(const QSize &kernel, const QSize &stride, const QSize &outputsize)
{
    if (!isSane(kernel,stride)) return QSize();
    const int w  = outputsize.width ()*stride.width (),
              h  = outputsize.height()*stride.height(),
              dw = kernel.width ()-stride.width (),
              dh = kernel.height()-stride.height();
    return QSize(w+dw,h+dh);
}
bool SSnConvHelper::isSane(const QSize &kernel, const QSize &stride)
{
    return (stride.width()>0)              && (stride.height()>0) &&
           (kernel.width()>0)              && (kernel.height()>0) &&
           (kernel.width()>=stride.width()) && (kernel.height()>=stride.height());
}
bool SSnConvHelper::isSane(const QSize &kernel, const QSize &stride, const QSize& outputsize)
{
    return isSane(kernel,stride) && (outputsize.width()>0) && (outputsize.height()>0);
}

QImage SSnConvHelper::scaledImage(const QImage &im, const QSize &kernel, const QSize &stride, const QSize &outputsize)
{
    if (isSane(kernel,stride,outputsize))
    {
        const auto sz = inputSize(kernel,stride,outputsize);
        if (im.size()==sz) return im;
        return im.scaled(sz,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    return QImage();
}
QSize SSnConvHelper::stride2Overlap(const QSize &kernel, const QSize &stride)
{
    return QSize(kernel.width()-stride.width(),kernel.height()-stride.height());
}
QSize SSnConvHelper::overlap2Stride(const QSize &kernel, const QSize &overlap)
{
    return stride2Overlap(kernel,overlap);
}
QList<QList<QRgb> > SSnConvHelper::matrix(const QImage &im, const QSize &kernel, const QSize &stride, const QSize &outputsize)
{
    QList<QList<QRgb> > ret;
    if (isSane(kernel,stride,outputsize))
    {
        foreach(const auto& ii, images(im,kernel,stride,outputsize))
        {
            QList<QRgb> l;
            for (int j=0; j<ii.height(); ++j) for (int i=0; i<ii.width(); ++i) l << ii.pixel(i,j);
            ret << l;
        }
    }
    return ret;
}


QList<QList<uchar> > SSnConvHelper::colorValues(const QList<QList<QRgb> >& matrix)
{
    QList<QList<uchar> > ret;
    QList<QRgb> line;
    foreach(line, matrix)
    {
        QList<uchar> l;
        foreach(auto c, line) l << (qRed   (c)&0xff);
        foreach(auto c, line) l << (qGreen (c)&0xff);
        foreach(auto c, line) l << (qBlue  (c)&0xff);
        ret << l;
    };
    return ret;
}
QList<QList<uchar> > SSnConvHelper::grayValues(const QList<QList<QRgb> >& matrix)
{
    QList<QList<uchar> > ret;
    QList<QRgb> line;
    foreach(line, matrix)
    {
        QList<uchar> l;
        foreach(auto c, line) l << (qGray(c)&0xff);
        ret << l;
    }
    return ret;
}


QImage SSnConvHelper::image(const QImage &im, const QSize &kernel, const QSize &stride, const QSize &outputsize, const QPoint &element)
{
    QImage ret(kernel,im.format());
    const QImage im2 = scaledImage(im,kernel,stride,outputsize);
    const auto tl = topLeftPosition(stride,element);
    foreach(auto p, convPositions(kernel,stride,element))
        ret.setPixel(p-tl,im2.pixel(p));
    return ret;
}
QList<QImage> SSnConvHelper::images(const QImage& im, const QSize& kernel, const QSize& stride, const QSize& outputsize)
{
    QList<QImage> ret;
    const QImage im2 = scaledImage(im,kernel,stride,outputsize);
    for (int j=0; j<outputsize.height(); ++j) for (int i=0; i<outputsize.width(); ++i)
         ret << image(im2,kernel,stride,outputsize,QPoint(i,j));
    return ret;
}

QSize SSnConvHelper::fitToInput(const QSize &input, const QSize &kernel, const QSize &stride)
{
    QSize ret(-1,-1);
    if (isSane(kernel,stride) && (kernel.width()<=input.width()) && (kernel.height()<=input.height()))
    {
        if (((input.width()-kernel.width())%stride.width()==0) && ((input.height()-kernel.height())%stride.height()==0))
        {
            const int nw = 1+((input.width ()-kernel.width ())/stride.width()),
                      nh = 1+((input.height()-kernel.height())/stride.height());
            ret =QSize(nw,nh);
        }
    }
    return ret;
}

QImage SSnConvHelper::toImage(const QVector<double> &v, const QSize &sz)
{
    const double pixels = sz.width()*sz.height();
    Q_ASSERT(v.size()==pixels);
    QImage im(sz, QImage::Format_RGB888);
    if (pixels==0)
    {
        im.fill(Qt::black);
        return im;
    }
    double max = qAbs(v.first());
    foreach(auto value, v) if (qAbs(value)>max) max = qAbs(value);
    const double scale =  255.00/max;
    int idx = -1;
    for (int y=0; y<sz.height(); ++y) for (int x=0; x<sz.width(); ++x)
    {
        const double value = v[++idx];
        const quint8 av = qAbs(value)*scale;
        auto c = (value>=0) ? qRgb(0,av,0) : qRgb(av,0,0);
        im.setPixel(x,y,c);
    }
    return im;
}
