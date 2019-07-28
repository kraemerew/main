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

bool SScConvNeuron::setKernel(SScKernel* krn, quint32 idx)
{
    Q_CHECK_PTR(krn);
    if (m_krn && (krn!=m_krn)) return false;
    m_krn   = krn;
    m_kidx  = idx;
    return true;
}

double SScConvNeuron::net()
{
    Q_CHECK_PTR(m_krn);
    return m_krn->net(m_kidx);
}

void SScConvNeuron::reset()
{
    Q_CHECK_PTR(m_krn);
    if (m_krn) m_krn->reset();
}

double  SScConvNeuron::icon(SSiHighwayNeuron* other)
{
    Q_CHECK_PTR(m_krn);
    SScConvNeuron* on = dynamic_cast<SScConvNeuron*>(other);
    return on ? m_krn->iconValue(on,this) : 0.0;
}

double SScConvNeuron::deltaw(double o)
{

    return -dedo()*o*m_act->dev()*m_act->gain()*(1.0-carry());

}
double SScConvNeuron::deltaw(SSiHighwayNeuron *n)
{
    return deltaw(n->out());
}

double SScConvNeuron::priv_dedo()
{
    Q_CHECK_PTR(m_krn);
    // dE/do_j = sum(l) dE/dnet_l * dnet_l/do_j = sum(l) dE/dnet_l wjl
    //         = sum(l) w_jl dE/do_l do_l/dnet_l
    //         = sum(l) w_jl dedo(l) act(l)'gain_l
    double ret = 0;
    foreach(SScConvNeuron* l, m_krn->fwdConnections(this))
    {
       ret+=m_krn->iconValue(this,l)*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
    }
    foreach(SSiHighwayNeuron* l, m_out)
    {
        ret += l->forwardSelectedDedo(this);    // only the pool neuron delivers something here, if this neuron achieved maximum in pool
        ret += l->icon(this)*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
    }
    return ret;
}
