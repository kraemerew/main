#include "sscneuron.hpp"
#include <QtMath>
SScNeuron::SScNeuron()
{
}

bool SScNeuron::addInput(SScNeuron *other, double v)
{
    if ((this==other) || m_in.contains(other)) return false;
    m_in[other]=v;
    return true;
}

bool SScNeuron::delInput(SScNeuron *other)
{
    if (!m_in.contains(other)) return false;
    m_in.remove(other);
    return true;
}

double SScNeuron::net() const
{
    double ret = 0;
    for(QMap<SScNeuron*,double>::const_iterator it = m_in.begin(); it != m_in.end(); ++it)
        ret += it.key()->out()*it.value();
    return ret;
}

double SScNeuron::out() const
{
    return 1.0/(1+exp(net()));
}
