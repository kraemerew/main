#include "conv.hpp"
#include "convunit.hpp"
#include "kernel.hpp"

SScConvNeuron::SScConvNeuron(SScHighwayNetwork *net)
    :   SSiHighwayNeuron(net, Conv, SScActivation::SWISH),
        m_krn         (NULL),
        m_kidx        (-1)
{}

SScConvNeuron::~SScConvNeuron()
{}

bool SScConvNeuron::setKernel(SScKernel* krn, int idx)
{
    Q_CHECK_PTR(krn);
    if (m_krn && (krn!=m_krn)) return false;
    Q_ASSERT(idx>=0);
    m_krn   = krn;
    m_kidx  = idx;
    return true;
}

double SScConvNeuron::net()
{
    Q_CHECK_PTR(m_krn);
    return m_krn->net(m_kidx);
}

double SScConvNeuron::priv_dedo()
{
    // dE/do_j = sum(l) dE/dnet_l * dnet_l/do_j = sum(l) dE/dnet_l wjl
    //         = sum(l) w_jl dE/do_l do_l/dnet_l
    //         = sum(l) w_jl dedo(l) act(l)'gain_l
    double ret = 0;
    foreach(SSiHighwayNeuron* l, m_out)
    {
        ret += l->forwardSelectedDedo(this);    // only the pool neuron delivers something here, if this neuron achieved maximum in pool
        ret += l->icon(this)*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
    }
    return ret;
}
