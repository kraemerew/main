#include "convhelpers.hpp"


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
        if ((qBound(0,x,w)==x) && (qBound(0,y,h)==y))
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
