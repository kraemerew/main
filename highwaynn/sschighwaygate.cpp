#include "sschighwaygate.hpp"
#include "highwayneuron.hpp"
#include "sschighwaynetwork.hpp"

SScHighwayGate::SScHighwayGate(SSiHighwayNeuron* parent)
    : QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >(),
      m_parent(parent), m_dirty(true), m_net(0.0)
{}

SScHighwayGate::~SScHighwayGate() {}

bool SScHighwayGate::addInput(SSiHighwayNeuron *other, SScTrainableParameter* tp)
{
    Q_CHECK_PTR(other);
    Q_CHECK_PTR(tp);
    if ((m_parent==other) || contains(other)) return false;
    m_dirty=true;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(tp);
    return true;
}
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

QVariantMap SScHighwayGate::toVM() const
{
    QVariantMap vm;
    foreach(SSiHighwayNeuron* n, this->keys())
    {
        vm[QString("CON_%1").arg(n->index())] = (*this)[n]->toVM();
    }
    return vm;
}
bool SScHighwayGate::fromVM(SScHighwayNetwork* net, const QVariantMap& vm)
{    
    clear();
    foreach(const QString& key, vm.keys()) if (key.startsWith("CON"))
    {
        bool ok = false;
        const int idx = key.split("_").last().toInt(&ok);
        if (idx>=0)
        {
            net->connect(idx,m_parent->index(),vm[key].toMap());
        }
    }
    return true;
}
