#include "sschighwaygate.hpp"
#include "sschighwayneuron.hpp"

SScHighwayGate::SScHighwayGate(SSiHighwayNeuron* parent)
    : QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >(),
      m_parent(parent), m_dirty(true), m_net(0.0) {}

SScHighwayGate::~SScHighwayGate() {}

bool SScHighwayGate::addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
{
    Q_CHECK_PTR(other);
    if ((m_parent==other) || contains(other)) return false;
    m_dirty=true;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(t,v));
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
       /* double net = 0;
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
            net += it.key()->out()*it.value()->value();
        m_net = net;
*/
        if (m_b.size()!=m_a.size()) endOfCycle();
        m_net = SSnBlas::dot(m_a,m_b);
        //qWarning("CALC NET %s %lf %lf", qPrintable(m_parent->name()), m_net, m_a.dot(m_b));
    }
    return m_net;
}

void SScHighwayGate::endOfCycle()
{
    m_b.clear();
    m_b.reserve(size());
    for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
    {
        it.value()->endOfCycle();
        m_b << it.value()->value();
    }
}
void SScHighwayGate::reset()
{
    m_dirty=true;
    m_a.clear();
    m_a.reserve(size());
    for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        m_a << it.key()->out();
}
