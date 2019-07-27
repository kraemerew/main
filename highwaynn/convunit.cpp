#include "convunit.hpp"
#include "sscvm.hpp"
#include "neurons/conv.hpp"
#include "../blas/blasvector.hpp"
#include "network.hpp"
#include "kernel.hpp"
#include <QUuid>
namespace SSnConvHelpers
{
/*!
     * \brief Return convolution mask as index
     * \param kx    Kernel size x
     * \param ky    Kernel size y
     * \param ovl   Overlap
     * \param xidx  Mask index x (at overlap 0, xidx*kx)
     * \param yidx  Mask index y
     * \param w     Source image width
     * \param h     Source image height
     * \return
     */
    QList<int> convMaskIdx(int kx, int ky, int ovl, int xidx, int yidx, int w, int h)
    {
        QList<int> ret;
        bool ok = (ky>0) && (ky>0) && (xidx>=0) && (yidx>=0) && (w>0) && (h>0);
        if (!ok) return ret;
        const int topleftx = kx + ((xidx-1)*(kx-ovl)),
                  toplefty = ky + ((yidx-1)*(ky-ovl));
        for (int y = toplefty; y<toplefty+ky; ++y) for (int x = topleftx; x<topleftx+kx; ++x)
        {
            if ((qBound(0,x,w)==x) && (qBound(0,y,h)==y))
                ret << x + (y*h);
            else ok = false;
        }
        if (!ok) ret.clear();
        return ret;
    }
    /*!
     * \brief Checks whether a convolution mask of kx,ky with overlap ovl fits into an image/array of wxh
     * \param kx    Applied kernel width
     * \param ky    Applied kernel height
     * \param ovl   Overlap in kernel
     * \param w     Source array width
     * \param h     Source array height
     * \return  Number of kernel units or invalid size on failure
     */
    QSize convMaskFits(int kx, int ky, int ovl, int w, int h)
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

    double max(const QVector<double>& v)
    {
        Q_ASSERT(!v.isEmpty());
        double ret = v[0];
        for (int i=0; i<v.size(); ++i) if (v[i]>ret) ret = v[i];
        return ret;
    }
    /*QVector<double> pooled(int xunits, int yunits, int pool, const QVector<double>& v)
    {
        if (pool<2) return v;
        Q_ASSERT(v.size()==xunits*yunits);
        Q_ASSERT(xunits%pool==0);
        Q_ASSERT(yunits%pool==0);
        QMap<int,QVector<double> > poolcache;
        const int newwidth = xunits/pool;
        int nr=-1;

        for (int y=0; y<yunits; ++y)
        {
            const int pooloffs = newwidth*(y/pool);
            for (int x=0; x<xunits; ++x)
            {
                const int poolx = x/pool;
                poolcache[pooloffs+poolx] << v[++nr];
            }
        }
        QVector<double> ret;
        ret.reserve(poolcache.size());
        foreach(int idx, poolcache.keys()) ret << SSnConvHelpers::max(poolcache[idx]);
        return ret;
    }*/
}

SSiConvUnit::SSiConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx, int unitsy, int overlap, int knr) :
    m_network   (network),
    m_kx        (kx),
    m_ky        (ky),
    m_unitsx    (unitsx),
    m_unitsy    (unitsy),
    m_ovl       (overlap)
{
    Q_CHECK_PTR(network);
    ensureCleanConf();
    createKernels(qMax(1,knr));
}
SSiConvUnit::~SSiConvUnit()
{
    clearKernels();
}

void SSiConvUnit::clearKernels  () { foreach(SScKernel* k, m_kernels) delete k; m_kernels.clear(); }
void SSiConvUnit::createKernels (int nr)
{
    while (m_kernels.size()<nr)
    {
        m_kernels << new (std::nothrow) SScKernel(m_network, m_kx*m_ky, m_unitsx*m_unitsy);
    }
}
void SSiConvUnit::ensureCleanConf()
{
    // odd kernel size min 3
    m_kx=qMax(3,m_kx); if (m_kx%2==0) ++m_kx;
    m_ky=qMax(3,m_ky); if (m_ky%2==0) ++m_ky;
}

QVariantMap SSiConvUnit::toVM() const
{
    QVariantMap vm;
    vm["KERNEL_X"] = m_kx;
    vm["KERNEL_X"] = m_ky;
    vm["KERNEL_OVERLAP"] = m_ovl;
    vm["UNITS_X"] = m_unitsx;
    vm["UNITS_Y"] = m_unitsy;
    vm["KERNELS"] = m_kernels.size();
    for (int i=0; i<m_kernels.size(); ++i) vm[QString("KERNEL_%1").arg(i)] = m_kernels[i]->toVM();
    return vm;
}
bool SSiConvUnit::fromVM(const QVariantMap & vm)
{
    bool ret = true;
    SScVM sscvm(vm);
    m_kx = sscvm.intToken("KERNEL_X",3);
    m_ky = sscvm.intToken("KERNEL_Y",3);
    m_unitsx = sscvm.intToken("UNITS_X",10);
    m_unitsy = sscvm.intToken("UNITS_Y",10);
    int knr = sscvm.intToken("KERNELS",1);
    knr = qMax(1,knr);
    clearKernels();
    ensureCleanConf();
    createKernels(knr);
    for (int i=0; i<knr; ++i) if (!m_kernels[i]->fromVM(sscvm.vmToken(QString("KERNEL_%1").arg(i)))) ret = false;
    return ret;
}

SScInputConvUnit::SScInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx, int unitsy, int overlap, int knr)
    : SSiConvUnit(network,kx,ky,unitsx,unitsy,overlap,knr)
{
    m_pp.reconfigure(xpixels(),ypixels(),isColor(),m_kx,m_ky,m_ovl);
}

SScInputConvUnit::~SScInputConvUnit()
{
    clearKernels();
}


QString SScInputConvUnit::nextPattern(bool& cycleDone)
{
    const QString key = m_pp.nextKey(cycleDone);
    if (activatePattern(key)) return key;
    return QString();
}

bool SScInputConvUnit::activatePattern(const QString& uuid)
{
    bool ret = false;
    if (m_pp.hasPattern(uuid))
    {
        ret = true;
        foreach(SScKernel* k, m_kernels) if (!k->activatePattern(m_pp.getPattern(uuid))) ret = false;
    }
    return ret;
}


SScColorInputConvUnit::SScColorInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx, int unitsy, int overlap,int knr)
    : SScInputConvUnit(network,kx,ky,unitsx,unitsy,overlap,knr)
{}
SScColorInputConvUnit::~SScColorInputConvUnit()
{
    clearKernels();
}


SScHiddenConvUnit::SScHiddenConvUnit(SScHighwayNetwork* network, int kx, int ky, int overlap, int knr)
    : SSiConvUnit(network,kx,ky,2,2,overlap,knr)
{}
SScHiddenConvUnit::~SScHiddenConvUnit()
{
    clearKernels();
}

bool SScHiddenConvUnit::connectFrom(SSiConvUnit* from)
{
    Q_CHECK_PTR(from);

    const int xu = from->xunits(), yu = from->yunits();
    const QSize out_xy = SSnConvHelpers::convMaskFits(m_kx,m_ky,m_ovl,xu,yu);
    if (!out_xy.isValid()) return false;
    m_from=from;
    m_unitsx = out_xy.width();
    m_unitsy = out_xy.height();
    for (int i=0; i<m_unitsx; ++i) for (int j=0; j<m_unitsy; ++j)
    {
        auto idxlist = SSnConvHelpers::convMaskIdx(m_kx,m_ky,m_ovl,i,j,xu,yu);
        if (idxlist.isEmpty()) return false;
        //TODO

    }
    return false;
}

QVariantMap SScInputConvUnit::toVM() const
{
    QVariantMap vm = SSiConvUnit::toVM();
    vm["TYPE"] = "INPUT";
    return vm;
}

QVariantMap SScColorInputConvUnit::toVM() const
{
    QVariantMap vm = SSiConvUnit::toVM();
    vm["TYPE"] = "INPUT_COLOR";
    return vm;
}

QVariantMap SScHiddenConvUnit::toVM() const
{
    QVariantMap vm = SSiConvUnit::toVM();
    vm["TYPE"] = "HIDDEN";
    return vm;
}

SSiConvUnit* SSiConvUnit::create(SScHighwayNetwork* net, const QVariantMap &vm)
{
    SSiConvUnit* ret = NULL;
    SScVM sscvm(vm);
    const QString type = sscvm.stringToken("TYPE");
    if (type=="INPUT")          ret = new (std::nothrow) SScInputConvUnit       (net,3,3); else
    if (type=="INPUT_COLOR")    ret = new (std::nothrow) SScColorInputConvUnit  (net,3,3); else
    if (type=="HIDDEN")         ret = new (std::nothrow) SScHiddenConvUnit      (net,3,3);
    if (ret) ret->fromVM(vm);
    return ret;
}
