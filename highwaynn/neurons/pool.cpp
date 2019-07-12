#include "pool.hpp"
#include "sscvm.hpp"
#include "carry.hpp"
#include "../network.hpp"

SScPoolNeuron::SScPoolNeuron(SScHighwayNetwork* net, SSiHighwayNeuron::Type t)
    : SSiHighwayNeuron  (net, t),
      m_selected        (false),
      m_sel             (NULL)
{
    Q_ASSERT ((t==MinPool) || (t==MaxPool));
    setActivation(SScActivation::IDENTITY,1.0);
}

double SScPoolNeuron::transform()
{
    if (!m_transformset)
    {
        m_t = m_act->activate(net());
        m_transformset = true;
    }
    return m_t;
}
bool SScPoolNeuron::addInput(SSiHighwayNeuron* other, SScTrainableParameter*)
{
    if (m_in.contains(other)) return true;
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    SScPoolNeuron*  pn = dynamic_cast<SScPoolNeuron*> (other);
    if (cn || pn) return false;
    m_in << other;
    return true;
}
bool SScPoolNeuron::addInput(SSiHighwayNeuron *other, double, SScTrainableParameter::Type)
{
    if (m_in.contains(other)) return true;
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    SScPoolNeuron* pn = dynamic_cast<SScPoolNeuron*>(other);
    if (cn || pn) return false;
    m_in << other;
    return true;
}
bool SScPoolNeuron::delInput(SSiHighwayNeuron *other)
{
    return m_in.removeAll(other);
}
double SScPoolNeuron::net()
{
    if (!m_selected)
    {
        m_selected = true;
        priv_poolselect();
    }
    return m_sel ? m_sel->out() : 0.0;
}

double SScPoolNeuron::forwardSelectedDedo(SSiHighwayNeuron* ref)
{
    if (m_sel!=ref) return false;
    if (m_sel==NULL) transform();

    // If ref is the neuron having achieved max, it takes effectively the place of the pool and
    // we behave as if it is directly connected to the neurons following the pool
    if (!m_dedoset)
    {
        m_dedo = 0;
        m_dedoset = true;
        foreach(SSiHighwayNeuron* l, m_out)
        {
            const double w_jl = l->icon(this);
            m_dedo += w_jl*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
        }
    }
    return m_dedo;
}

QVariantMap SScPoolNeuron::toVM() const
{
    QVariantMap vm = SSiHighwayNeuron::toVM();
    QVariantList il;
    foreach(SSiHighwayNeuron* n, m_in) il << n->index();

    vm["GATE"] = il;
    return vm;
}
bool SScPoolNeuron::fromVM(const QVariantMap &vm)
{
    bool ret = SSiHighwayNeuron::fromVM(vm);
    m_in.clear();
    SScVM sscvm(vm);
    auto vl = sscvm.vlToken("GATE");
    foreach(const auto& v, vl) if (!v.canConvert<int>()) ret = false; else
    {
        if (!m_net->connect(v.toInt(),index())) ret = false;
    }
    return ret;
}

void SScPoolNeuron::dump()
{
    SSiHighwayNeuron::dump();
    qWarning("> %d connections", m_in.size());
}
