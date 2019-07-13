#include "gate.hpp"
#include "neuron.hpp"
#include "network.hpp"

SScGate::SScGate(SSiHighwayNeuron* parent)
    : QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >(),
      m_parent(parent), m_dirty(true), m_net(0.0)
{}

SScGate::~SScGate() {}

bool SScGate::addInput(SSiHighwayNeuron *other, SScTrainableParameter* tp)
{
    Q_CHECK_PTR(other);
    Q_CHECK_PTR(tp);
    if ((m_parent==other) || contains(other)) return false;
    m_dirty=true;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(tp);
    return true;
}
bool SScGate::addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
{
    Q_CHECK_PTR(other);
    if ((m_parent==other) || contains(other)) return false;
    m_dirty=true;
    (*this)[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(t,v));
    return true;
}
bool SScGate::delInput(SSiHighwayNeuron *other)
{
    Q_CHECK_PTR(other);
    if (!contains(other)) return false;
    m_dirty=true;
    remove(other);
    return true;
}
double SScGate::net()
{
    if (m_dirty)
    {
        QVector<double> a, b;
        a.reserve(size());
        b.reserve(size());
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        {
            qWarning("???? %s IN FROM %s %lf*%lf = %lf", qPrintable(m_parent->name()), qPrintable(it.key()->name()), it.value()->value(), it.key()->out(), it.value()->value()*it.key()->out());
            a << it.key()->out();
            b << it.value()->value();
        }

        m_dirty = false;
        m_net = SSnBlas::dot(a,b);
    }
    return m_net;
}

void SScGate::endOfCycle()
{
    for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = begin(); it != end(); ++it)
        it.value()->endOfCycle();    
}
void SScGate::reset()
{
    m_dirty=true;
}

QVariantMap SScGate::toVM() const
{
    QVariantMap vm;
    foreach(SSiHighwayNeuron* n, this->keys())
    {
        vm[QString("CON_%1").arg(n->index())] = (*this)[n]->toVM();
    }
    return vm;
}
bool SScGate::fromVM(SScHighwayNetwork* net, const QVariantMap& vm)
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
