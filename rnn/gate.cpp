#include "gate.hpp"
#include "rneuron.hpp"

SScGate::SScGate(SScActivation::SSeActivation act)
    : m_act     (SScActivation::create(act)),
      m_zeroact (m_act->activate(0.0))
{}

SScGate::~SScGate()
{
    delete m_act;
    foreach(SScRNeuron* n, m_con.keys()) delConnection(n);
}
bool SScGate::addConnection(SScRNeuron *n, double w, SScConnection::SSeConnectionType ctype)
{
    Q_CHECK_PTR(n);
    if (!m_con.contains(n))
    {
        m_con[n] = new (std::nothrow) SScConnection(ctype,w);
        Q_CHECK_PTR(m_con[n]);
        return true;
    }
    return false;
}

bool SScGate::delConnection(SScRNeuron* n)
{
    if (m_con.contains(n))
    {
        delete m_con[n];
        m_con.remove(n);
        return true;
    }
    return false;
}

double SScGate::get(int t)
{
    if (t<0) return m_zeroact;
    if (!m_signal.contains(t))
    {
        double sum = 0.0;
        for (QHash<SScRNeuron*,SScConnection*>::iterator it = m_con.begin(); it!=m_con.end(); ++it)
            sum+=it.key()->out(t-1)*it.value()->value();
        m_signal[t]=m_act->activate(sum);
    }
    return m_signal[t];
}
