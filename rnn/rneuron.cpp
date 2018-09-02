#include "rneuron.hpp"
#include "activation/sscactivation.hpp"

SScRNeuron::SScRNeuron(SScActivation::SSeActivation acttype)
    : SScRNeuronBase(),
      m_act         (SScActivation::create(acttype)),
      m_zeroact     (m_act->activate(0.0)),
      m_isig        (NULL),
      m_tsig        (NULL),
      m_ginput      (new (std::nothrow) SScGate(SScActivation::Act_Tanh)),
      m_goutput     (new (std::nothrow) SScGate(SScActivation::Act_Logistic)),
      m_gmemory     (new (std::nothrow) SScGate(SScActivation::Act_Logistic)),
      m_gstore      (new (std::nothrow) SScGate(SScActivation::Act_Logistic))
{
    Q_CHECK_PTR(m_ginput);
    Q_CHECK_PTR(m_goutput);
    Q_CHECK_PTR(m_gmemory);
    Q_CHECK_PTR(m_gstore);
}

SScRNeuron::~SScRNeuron()
{
    delete m_act;
    delete m_ginput;
    delete m_goutput;
    delete m_gmemory;
    delete m_gstore;
}

void SScRNeuron::clear()
{
    m_osig.     clear();
    m_msig.     clear();
    m_ginput  ->clear();
    m_goutput ->clear();
    m_gmemory ->clear();
    m_gstore  ->clear();
}

double SScRNeuron::err(int t)
{
    return m_tsig ? out(t)-m_tsig->at(t) : 0.0;
}

double SScRNeuron::in(int t)
{
    return m_isig ? (m_isig->at(t)+m_ginput->get(t)) : m_ginput->get(t);
}

double SScRNeuron::out(int t)
{
    if (t<0) return m_zeroact;

    if (!m_osig.contains(t))
    {
        m_osig[t]=mem(t)*m_goutput->get(t);
    }
    return m_osig[t];
}

double SScRNeuron::mem(int t)
{
    if (t<0) return 0.0;
    if (!m_msig.contains(t))
    {
        m_msig[t] = mem(t-1)*m_gmemory->get(t)+in(t)*m_gstore->get(t);
    }
    return m_msig.at(t);
}
