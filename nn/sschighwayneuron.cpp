#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


SScHighwayNeuron::SScHighwayNeuron(SScHighwayNetwork* nw, int layer, int nr)
    : SScNeuron (SScNeuron::NeuronType_Hidden),
      m_nw      (nw),
      m_layer   (layer),
      m_nr      (nr),
      m_in      (this),
      m_cin     (this),
      m_act     (SScActivation::create(SScActivation::ACT_TANH)),
      m_cact    (SScActivation::create(SScActivation::ACT_SIGMOID)),
      m_hw      (NULL)
{
    Q_CHECK_PTR(nw);
}

SScHighwayNeuron::~SScHighwayNeuron()
{
    delete m_act;
    delete m_cact;
}
