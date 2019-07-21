#include "convunit.hpp"
#include "sscvm.hpp"
#include "../blas/blasvector.hpp"
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



SSiConvUnit::SSiConvUnit(int kx, int ky, int unitsx, int unitsy, int overlap, int pooling) :
    m_kx        (kx),
    m_ky        (ky),
    m_unitsx    (unitsx),
    m_unitsy    (unitsy),
    m_ovl       (overlap),
    m_pooling   (qMax(1,pooling))
{
    ensureCleanConf();
    createWeights();
}
SSiConvUnit::~SSiConvUnit()
{
    clearWeights();
}
void SSiConvUnit::createWeights()
{
    clearWeights();
    for (int i=0; i<weights(); ++i) m_w << SScTrainableParameter::create(SScTrainableParameter::ADAM, (double)qrand()/(double)RAND_MAX); //<< TODO - reset
}
void SSiConvUnit::clearWeights()
{
    foreach(SScTrainableParameter* tp, m_w) delete tp;
    m_w.clear();
}
void SSiConvUnit::ensureCleanConf()
{
    // odd kernel size min 3
    m_kx=qMax(3,m_kx); if (m_kx%2==0) ++m_kx;
    m_ky=qMax(3,m_ky); if (m_ky%2==0) ++m_ky;

    if (m_pooling>1)  // number of units have to be multiples of pooling
    {
        while (m_unitsx%m_pooling!=0) ++m_unitsx;
        while (m_unitsy%m_pooling!=0) ++m_unitsy;
    }
}

QVariantMap SSiConvUnit::toVM() const
{
    QVariantMap vm;
    vm["KERNEL_X"] = m_kx;
    vm["KERNEL_X"] = m_ky;
    vm["KERNEL_OVERLAP"] = m_ovl;
    vm["POOLING"] = m_pooling;
    vm["UNITS_X"] = m_unitsx;
    vm["UNITS_Y"] = m_unitsy;
    int widx = -1;
    foreach(SScTrainableParameter* tp, m_w)
        vm[QString("WEIGHT_%1").arg(++widx)] = tp->toVM();
    return vm;
}
bool SSiConvUnit::fromVM(const QVariantMap & vm)
{
    bool ret = true;
    SScVM sscvm(vm);
    m_kx = sscvm.intToken("KERNEL_X",3);
    m_ky = sscvm.intToken("KERNEL_Y",3);
    m_pooling = sscvm.intToken("POOLING",1);
    m_unitsx = sscvm.intToken("UNITS_X",10);
    m_unitsy = sscvm.intToken("UNITS_Y",10);
    clearWeights();
    ensureCleanConf();
    for (int widx=0; widx<weights(); ++widx)
    {
        SScTrainableParameter* tp = SScTrainableParameter::create(sscvm.vmToken(QString("WEIGHT_%1").arg(widx)));
        if (!tp)
        {
            tp = SScTrainableParameter::create(SScTrainableParameter::ADAM, (double)qrand()/(double)RAND_MAX); //<< TODO - reset
            ret = false;
        }
        m_w << tp;
    }
    return ret;
}

SScInputConvUnit::SScInputConvUnit(int kx, int ky, int unitsx, int unitsy, int overlap, int pooling)
    : SSiConvUnit(kx,ky,unitsx,unitsy,overlap,pooling)
{}

SScInputConvUnit::~SScInputConvUnit()
{
    clearWeights();
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
    SScConvPattern p(im);
    p.convert(xpixels(),ypixels());
    auto key = QUuid::createUuid().toString();
    m_images[key]=p.m_im;
    for (int y=0; y<m_unitsy; ++y) for (int x = 0; x<m_unitsx; ++x)
    {
        const int topleftx = x*m_kx-x*m_ovl, toplefty = y*m_ky-y*m_ovl;
        auto v = p.vect(topleftx, toplefty, m_kx, m_ky, isColor());
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

QString SScInputConvUnit::nextPattern(bool& cycleDone)
{
    cycleDone = false;
    if (!m_pkeys.isEmpty())
    {
        const QString key = m_pkeys.takeFirst();
        m_pkeys << key;
        if (activatePattern(key))
        {   if (key==m_patterns.lastKey()) cycleDone = true;
            return key;
        }
    }
    return QString();
}
bool SScInputConvUnit::activatePattern(const QString& uuid)
{
    if (m_patterns.contains(uuid) && (m_patterns[uuid].size()==units()))
    {
        QVector<double> w;
        w.reserve(m_w.size());
        for (int i=0; i<m_w.size(); ++i) w << m_w[i]->value();
        m_n.clear();
        m_n.reserve(units());

        foreach(const QVector<double>& dv, m_patterns[uuid])
        {
            m_n << SSnBlas::dot(w,dv);
            m_npooled = SSnConvHelpers::pooled(m_unitsx,m_unitsy,m_pooling,m_n);
        }
        return true;
    }
    return false;
}


SScColorInputConvUnit::SScColorInputConvUnit(int kx, int ky, int unitsx, int unitsy, int overlap, int pooling)
    : SScInputConvUnit(kx,ky,unitsx,unitsy,overlap,pooling)
{}
SScColorInputConvUnit::~SScColorInputConvUnit()
{}
