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
        QVector<double> a, b;
        a.reserve(size());
        b.reserve(size());
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        {
            a << it.key()->out();
            b << it.value()->value();
        }

        m_dirty = false;
        m_net = SSnBlas::dot(a,b);
    }
    return m_net;
}

void SScHighwayGate::endOfCycle()
{
    for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        it.value()->endOfCycle();    
}
void SScHighwayGate::reset()
{
    m_dirty=true;
}
