#include "sscgate.hpp"
#include "sscneuron.hpp"

SScGate::SScGate(SScNeuron* parent) : QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> >(), m_parent(parent) {}

SScGate::~SScGate() {}

bool SScGate::addInput(SScNeuron *other, double v)
{
    Q_CHECK_PTR(other);
    if ((m_parent==other) || contains(other)) return false;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(SScTrainableParameter::CON_RPROP,v));
    return true;
}
bool SScGate::delInput(SScNeuron *other)
{
    Q_CHECK_PTR(other);
    if (!contains(other)) return false;
    remove(other);
    return true;
}
double SScGate::net()
{
    double ret = 0;
    for(QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        ret += it.key()->out()*it.value()->value();
    return ret;
}

