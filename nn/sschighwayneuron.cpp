#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


SScHighwayStdNeuron::SScHighwayStdNeuron(SScHighwayNetwork* nw, int layer, int nr)
    : SScNeuron (SScNeuron::NeuronType_Hidden),
      m_nw      (nw),
      m_layer   (layer),
      m_nr      (nr),
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

SScHighwayNeuron::SScHighwayNeuron(SScHighwayNetwork* nw, int layer, int nr)
    : SScHighwayStdNeuron(nw,layer,nr),
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
