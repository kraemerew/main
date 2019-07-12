#include "pool.hpp"
#include "sscvm.hpp"
#include "carry.hpp"

SScPoolNeuron::SScPoolNeuron(SScHighwayNetwork* net)
    : SSiHighwayNeuron(net, Pool),
      m_sel(NULL)
{
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
    SScPoolNeuron* pn = dynamic_cast<SScPoolNeuron*>(other);
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
bool SScPoolNeuron::delInput(SSiHighwayNeuron *other) { return m_in.removeAll(other); }
double SScPoolNeuron::net()
{
    double ret = 0;
    m_sel = NULL;
    if (!m_in.isEmpty())
    {
        m_sel = m_in.first();
        ret   = m_sel->out();
        for(int i=1; i<m_in.size(); ++i) if (m_in[i]->out()>ret)
        {
            m_sel = m_in[i];
            ret   = m_sel->out();
        }
    }
    return ret;
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
    QList<int> il;
    foreach(SSiHighwayNeuron* n, m_in) il << n->index();
    //vm["GATE"] = il;
    return vm;
}
bool SScPoolNeuron::fromVM(const QVariantMap &vm)
{
    bool ret = SSiHighwayNeuron::fromVM(vm);
    m_in.clear();
    SScVM sscvm(vm);

    //TODO POOLING: if (!m_in.fromVM(m_net,sscvm.vmToken("GATE"))) ret = false;
    return ret;
}

void SScPoolNeuron::dump()
{
    SSiHighwayNeuron::dump();
    qWarning("> %d connections", m_in.size());
}
