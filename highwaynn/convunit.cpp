#include "convunit.hpp"
#include "sscvm.hpp"
#include "neurons/conv.hpp"
#include "../blas/blasvector.hpp"
#include "network.hpp"
#include "kernel.hpp"
#include "convhelpers.hpp"

#include <QUuid>
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

SScConvNeuron* SSiConvUnit::output(quint32 k, quint32 nidx) const { return (k<(quint32)m_kernels.size()) ? m_kernels[k]->output(nidx) : NULL; }

void SSiConvUnit::clearKernels  () { foreach(SScKernel* k, m_kernels) delete k; m_kernels.clear(); }
void SSiConvUnit::createKernels (int nr)
{
    while (m_kernels.size()<nr)
    {
        m_kernels << new (std::nothrow) SScKernel(m_network, weights(), units());
    }
}
void SSiConvUnit::ensureCleanConf()
{
    m_kx=qMax(1,m_kx);
    m_ky=qMax(1,m_ky);
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
    m_kx     = sscvm.intToken("KERNEL_X",3);
    m_ky     = sscvm.intToken("KERNEL_Y",3);
    m_unitsx = sscvm.intToken("UNITS_X",10);
    m_unitsy = sscvm.intToken("UNITS_Y",10);
    int knr  = sscvm.intToken("KERNELS", 1);
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
{
    Q_CHECK_PTR(network);
    ensureCleanConf();
    m_pp.reconfigure(xpixels(),ypixels(),isColor(),m_kx,m_ky,m_ovl);
    clearKernels();
    createKernels(qMax(1,knr));
}
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

    const int krn = from->kernels(), xu = from->xunits(), yu = from->yunits();

    const QSize out_xy = SSnConvHelpers::convMaskFits(m_kx,m_ky,m_ovl,xu,yu);
    if (!out_xy.isValid()) return false;
    m_from=from;
    m_unitsx = out_xy.width();
    m_unitsy = out_xy.height();
    clearKernels();
    createKernels(krn);       // as many kernels as the underlying connection
    for (int k=0; k<krn; ++k) for (int j=0; j<m_unitsy; ++j) for (int i=0; i<m_unitsx; ++i)
    {
        auto idxlist = SSnConvHelpers::convMaskIndexes(m_kx,m_ky,m_ovl,i,j,xu,yu);
        if (idxlist.isEmpty()) return false;
        QVector<SScConvNeuron*> field;
        foreach(int idx, idxlist) field << from->output(k,idx);
        m_kernels[krn]->addInputField(field);
    }
    return true;
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
