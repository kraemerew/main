#include "sschighwaygate.hpp"
#include "sschighwayneuron.hpp"

SScHighwayGate::SScHighwayGate(SSiHighwayNeuron* parent) : QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >(), m_parent(parent) {}

SScHighwayGate::~SScHighwayGate() {}

bool SScHighwayGate::addInput(SSiHighwayNeuron *other, double v)
{
    Q_CHECK_PTR(other);
    if ((m_parent==other) || contains(other)) return false;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(SScTrainableParameter::CON_RPROP,v));
    return true;
}
bool SScHighwayGate::delInput(SSiHighwayNeuron *other)
{
    Q_CHECK_PTR(other);
    if (!contains(other)) return false;
    remove(other);
    return true;
}
double SScHighwayGate::net()
{
    double net = 0;
    for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        net += it.key()->out()*it.value()->value();
    return net;
}

