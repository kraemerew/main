#include "sharedgate.hpp"
#include "ssctrainableparameter.hpp"

SScSharedGate::SScSharedGate(QVector<SSiHighwayNeuron*> parents)
    : QVector<SScTrainableParameter*>(),
      m_parents(parents),
      m_dirty(true)
{}

SScSharedGate::~SScSharedGate()
{
    priv_clear();
}

void SScSharedGate::priv_clear()
{
    foreach(SScTrainableParameter* p, *this) delete p;
}
QVector<double> SScSharedGate::net()
{
    if (m_dirty)
    {
        QVector<double> a, b;
        a.reserve(size()*m_parents.size());
        b.reserve(size());
        foreach(SScTrainableParameter* p, (*this))
            b << p->value();
        m_dirty = false;
        //TODO: vector-matrix blas product
        //m_net = SSnBlas::dot(a,b);
    }
    return m_net;
}

void SScSharedGate::endOfCycle()
{
    foreach(SScTrainableParameter* p, (*this))
        p->endOfCycle();
}
void SScSharedGate::reset()
{
    m_dirty=true;
}

QVariantMap SScSharedGate::toVM() const
{
    QVariantMap vm;
    int cidx = -1;
    foreach(SScTrainableParameter* p, (*this))
    {
        vm[QString("CON_%1").arg(++cidx)] = p->toVM();
    }
    return vm;
}
bool SScSharedGate::fromVM(const QVariantMap& vm)
{
    QMap<int,SScTrainableParameter*> pars;
    foreach(const QString& key, vm.keys()) if (key.startsWith("CON"))
    {

        bool ok = false;
        const int idx = key.split("_").last().toInt(&ok);
        if (idx>=0)
        {
            SScTrainableParameter* p = SScTrainableParameter::create(vm);
            if (p)
            {
                if (pars.contains(idx)) delete pars[idx];
                pars[idx]=p;
            }
        }
    }
    clear();
    auto pv = pars.values().toVector();
    reserve(pv.size());
    while (!pv.isEmpty()) (*this) << pv.takeFirst();
    return true;
}
