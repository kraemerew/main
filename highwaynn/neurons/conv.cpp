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
