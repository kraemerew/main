#include "convunitv2.hpp"
#include "sscvm.hpp"
#include "neurons/conv.hpp"
#include "../blas/blasvector.hpp"
#include "network.hpp"
#include "kernel.hpp"
#include "convhelpers.hpp"
#include "imageprovider.hpp"
#include <QUuid>

SSiConvUnit::SSiConvUnit(SScHighwayNetwork *network, const QSize &kern, const QSize &stride, const QSize &elements, int knr)
  : m_network   (network),
    m_ksz       (kern),
    m_str       (stride),
    m_units     (elements),
    m_knr       (knr)
{
    Q_CHECK_PTR(network);
}
SSiConvUnit::~SSiConvUnit()
{
    clearKernels();
}

void SSiConvUnit::reset             ()  { foreach(auto k, m_kernels) k->reset(); }
void SSiConvUnit::resetTraining     ()  { foreach(auto k, m_kernels) k->resetTraining(); }
void SSiConvUnit::clearKernels      ()  { foreach(auto k, m_kernels) delete k; m_kernels.clear(); }
void SSiConvUnit::createKernels     ()  { while (m_kernels.size()>m_knr) m_kernels << new SScKernel(m_network,m_ksz,m_str,m_units,depth()); }

SScConvNeuron* SSiConvUnit::output(quint32 k, quint32 nidx) const { return (k<(quint32)m_kernels.size()) ? m_kernels[k]->output(nidx) : NULL; }

QVariantMap SSiConvUnit::toVM() const
{
    QVariantMap vm;
    vm["TYPE"] = type();
    vm["KERNEL_X"] = m_ksz.     width();
    vm["KERNEL_X"] = m_ksz.     height();
    vm["UNITS_X"]  = m_units.   width();
    vm["UNITS_Y"]  = m_units.   height();
    vm["STRIDE_X"] = m_str.     width();
    vm["STRIDE_Y"] = m_str.     height();
    vm["KERNELS"]  = m_knr;
    for (int i=0; i<m_kernels.size(); ++i) vm[QString("KERNEL_%1").arg(i)] = m_kernels[i]->toVM();
    return vm;
}
bool SSiConvUnit::fromVM(const QVariantMap & vm)
{
    bool ret = true;
    SScVM sscvm(vm);
    int kx = sscvm.intToken("KERNEL_X",3),
        ky = sscvm.intToken("KERNEL_Y",3),
        ux = sscvm.intToken("UNITS_X",10),
        uy = sscvm.intToken("UNITS_Y",10),
        sx = sscvm.intToken("STRIDE_X", 1),
        sy = sscvm.intToken("STRIDE_Y", 1),
       knr = sscvm.intToken("KERNELS", 1);
    kx = qMax(1,kx);
    ky = qMax(1,ky);
    sx = qMax(1,sx);
    sy = qMax(1,sy);
    ux = qMax(1,ux);
    uy = qMax(1,uy);
    knr = qMax(1,knr);

    clearKernels();
    m_str   =   QSize(sx,sy);
    m_units =   QSize(ux,uy);
    m_ksz   =   QSize(kx,ky);
    m_knr   =   knr;
    createKernels();
    for (int i=0; i<knr; ++i) if (!m_kernels[i]->fromVM(sscvm.vmToken(QString("KERNEL_%1").arg(i)))) ret = false;
    return ret;
}

SScInputConvUnit::SScInputConvUnit(SScHighwayNetwork *network, const QSize &kern, const QSize &stride, const QSize &elements, int knr)
    : SSiConvUnit(network,kern,stride,elements,knr)
    , m_ip(NULL)
{
    if (network) m_ip = network->ip();
    Q_CHECK_PTR(m_ip);
    createKernels();
}

SScInputConvUnit::~SScInputConvUnit()
{
    clearKernels();
}

QString SScInputConvUnit::nextPattern(bool& cycleDone)
{
    const QString ret = m_ip->next(cycleDone);
    reset();
    return ret;
}

QString SScInputConvUnit::addPattern(const QImage& im) { return m_ip->append(im); }
QString SScInputConvUnit::addPattern(const QString& filename) { return m_ip->append(filename); }

SScColorInputConvUnit::SScColorInputConvUnit(SScHighwayNetwork *network, const QSize &kern, const QSize &stride, const QSize &elements, int knr)
    : SScInputConvUnit(network,kern,stride,elements,knr)
{
    createKernels();
}
SScColorInputConvUnit::~SScColorInputConvUnit()
{
    clearKernels();
}


SSiConvUnit* SSiConvUnit::create(SScHighwayNetwork* net, const QVariantMap &vm)
{
    SSiConvUnit* ret = NULL;
    SScVM sscvm(vm);
    const QString type = sscvm.stringToken("TYPE");
    if (type=="INPUT")          ret = new (std::nothrow) SScInputConvUnit       (net,QSize(3,3),QSize(1,1),QSize(1,1)); else
    if (type=="INPUT_COLOR")    ret = new (std::nothrow) SScColorInputConvUnit  (net,QSize(3,3),QSize(1,1),QSize(1,1)); //else
   // if (type=="HIDDEN")         ret = new (std::nothrow) SScHiddenConvUnit      (net,3,3);
    if (ret) ret->fromVM(vm);
    return ret;
}
