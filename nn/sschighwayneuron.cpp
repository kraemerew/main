#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


SScHighwayStdNeuron::SScHighwayStdNeuron(SScHighwayNetwork* nw)
    : SScNeuron (SScNeuron::NeuronType_Hidden),
      m_nw      (nw),
      m_in      (this),
      m_act     (SScActivation::create(SScActivation::ACT_TANH))
{
    Q_CHECK_PTR(nw);
}

SScHighwayStdNeuron::~SScHighwayStdNeuron()
{
    if (m_act) delete m_act;
    m_act=NULL;
}

SScHighwayNeuron::SScHighwayNeuron(SScHighwayNetwork* nw)
    : SScHighwayStdNeuron(nw),
      m_cin     (this),
      m_cact    (SScActivation::create(SScActivation::ACT_SIGMOID)),
      m_hw      (NULL)
{
    Q_CHECK_PTR(nw);
}

SScHighwayNeuron::~SScHighwayNeuron()
{
    if (m_act) delete m_act;
    m_act=NULL;
    delete m_cact;
}
