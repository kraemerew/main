#include "convhelpers.hpp"

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





QSize SSnConvHelpers::inputSize(int kx, int ky, int ovl, int xunits, int yunits)
{
    if ((kx>0) && (ky>0) && (xunits>0) && (yunits>0) && (ovl>=0))
        return QSize(kx+(xunits-1)*(kx-ovl), ky+ (yunits-1)*(ky-ovl));
    return QSize(-1,-1);
}

QList<int> SSnConvHelpers::convMaskIndexes(int kx, int ky, int ovl, int xidx, int yidx, int w, int h)
{
    QList<int> ret;
    foreach(const auto& pos, convMaskPositions(kx,ky,ovl,xidx,yidx,w,h))
        ret << pos.x()+(pos.y()*w);
    return ret;
}

QList<QPoint> SSnConvHelpers::convMaskPositions(int kx, int ky, int ovl, int xidx, int yidx, int w, int h)
{
    QList<QPoint> ret;
    bool ok = (ky>0) && (ky>0) && (xidx>=0) && (yidx>=0) && (w>0) && (h>0);
    if (!ok) return ret;
    const int topleftx = (kx*xidx)-(xidx*ovl),
              toplefty = (ky*yidx)-(yidx*ovl);
    for (int y = toplefty; y<toplefty+ky; ++y) for (int x = topleftx; x<topleftx+kx; ++x)
        if ((qBound(0,x,w)==x) && (qBound(0,y,h)==y))   //< should always be the case
            ret << QPoint(x,y);
        else ok = false;

    if (!ok) ret.clear();
    return ret;
}

QSize SSnConvHelpers::convMaskFits(int kx, int ky, int ovl, int w, int h)
{
    // kx + (n-1)(kx-ovl)==w
    // ky + (m-1)(ky-ovl)==h
    const int n = 1 + ((w-kx)/(kx-ovl)),
              m = 1 + ((h-ky)/(ky-ovl));

    // Check whether it fits
    const int wnew = kx + (n-1)*(kx-ovl),
              hnew = ky + (m-1)*(ky-ovl);

    // Has to be same size
    if ((w==wnew) && (h==hnew)) return QSize(n,m);
    return QSize(-1,-1);
}
