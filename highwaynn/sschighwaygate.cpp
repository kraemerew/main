#include "sschighwaygate.hpp"
#include "sschighwayneuron.hpp"

SScHighwayGate::SScHighwayGate(SSiHighwayNeuron* parent)
    : QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >(),
      m_parent(parent), m_dirty(true), m_net(0.0) {}

SScHighwayGate::~SScHighwayGate() {}

bool SScHighwayGate::addInput(SSiHighwayNeuron *other, double v)
{
    Q_CHECK_PTR(other);
    if ((m_parent==other) || contains(other)) return false;
    m_dirty=true;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(SScTrainableParameter::CON_RPROP,v));
    return true;
}
bool SScHighwayGate::delInput(SSiHighwayNeuron *other)
{
    Q_CHECK_PTR(other);
    if (!contains(other)) return false;
    m_dirty=true;
    remove(other);
    return true;
}
double SScHighwayGate::net()
{
    if (m_dirty)
    {
        m_dirty = false;
        double net = 0;
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        net += it.key()->out()*it.value()->value();
        m_net = net;
        //qWarning("CALC NET %s %lf", qPrintable(m_parent->name()), m_net);
    }
    //else qWarning("NET %s ALREADY DONE: %lf", qPrintable(m_parent->name()), m_net);
    return m_net;
}

void SScHighwayGate::reset()
{
    m_dirty=true;
}
