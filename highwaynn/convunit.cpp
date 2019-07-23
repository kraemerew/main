#include "convunit.hpp"
#include "sscvm.hpp"
#include "neurons/conv.hpp"
#include "../blas/blasvector.hpp"
#include "network.hpp"
#include "kernel.hpp"
#include <QUuid>
namespace SSnConvHelpers
{
    double max(const QVector<double>& v)
    {
        Q_ASSERT(!v.isEmpty());
        double ret = v[0];
        for (int i=0; i<v.size(); ++i) if (v[i]>ret) ret = v[i];
        return ret;
    }
    QVector<double> pooled(int xunits, int yunits, int pool, const QVector<double>& v)
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
    }
}

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
{}

SScInputConvUnit::~SScInputConvUnit()
{
    clearKernels();
}
QString SScInputConvUnit::addPattern(const QString& filename)
{
    QImage im;
    im.load(filename);
    return !im.isNull() ? addPattern(im) : QString();
}
QString SScInputConvUnit::addPattern(const QImage &im)
{
    bool success = true;
    SScConvPattern p(im,xpixels(),ypixels(),isColor());
    auto key = QUuid::createUuid().toString();
    m_images[key]=p.m_im;
    qWarning(">>ADDED %dx%d %dbpp", p.m_im.width(), p.m_im.height(), p.m_im.depth());
    for (int y=0; y<m_unitsy; ++y) for (int x = 0; x<m_unitsx; ++x)
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

QString SScInputConvUnit::nextPattern(bool& cycleDone)
{
    cycleDone = false;
    if (!m_pkeys.isEmpty())
    {
        const QString key = m_pkeys.takeFirst();
        m_pkeys << key;
        if (activatePattern(key))
        {
            if (key==m_patterns.lastKey()) cycleDone = true;
            return key;
        }
    }
    return QString();
}

bool SScInputConvUnit::activatePattern(const QString& uuid)
{
    bool ret = false;
    if (m_patterns.contains(uuid))
    {
        ret = true;
        foreach(SScKernel* k, m_kernels) if (!k->activatePattern(m_patterns[uuid])) ret = false;
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


SScHiddenConvUnit::SScHiddenConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx, int unitsy, int overlap, int knr)
    : SSiConvUnit(network,kx,ky,unitsx,unitsy,overlap,knr)
{}
SScHiddenConvUnit::~SScHiddenConvUnit()
{
    clearKernels();
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