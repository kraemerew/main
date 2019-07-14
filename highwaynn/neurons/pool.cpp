#include "pool.hpp"
#include "sscvm.hpp"
#include "carry.hpp"
#include "../network.hpp"

SScPoolNeuron::SScPoolNeuron(SScHighwayNetwork* net, SSiHighwayNeuron::Type t)
    : SSiHighwayNeuron  (net, t),
      m_selected        (false),
      m_fwdedoset       (false),
      m_fwdedo          (0.0),
      m_sel             (NULL)
{
    Q_ASSERT ((t==MinPool) || (t==MaxPool) || (t==MedPool));
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
    if (!m_sel || (m_sel!=ref)) return 0.0;
    if (m_sel==NULL) transform();
    // If ref is the neuron selected, it takes effectively the place of the pool and
    // we behave as if it is directly connected to the neurons following the pool
    if (!m_fwdedoset)
    {
        m_fwdedo = 0;
        m_fwdedoset = true;
        foreach(SSiHighwayNeuron* l, m_out)
        {
            const double w_jl = l->icon(this);
            m_fwdedo += w_jl*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
        }
    }
    return m_fwdedo;
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


class SScSortableNeuron
{
public:
    SScSortableNeuron(SSiHighwayNeuron* n) : m_n(n), m_o(n->out()) {}
    bool operator < (const SScSortableNeuron& other) const { return m_o < other.m_o; }

    SSiHighwayNeuron* m_n;
    double m_o;
};

void SScMaxPoolNeuron::priv_poolselect()
{
    m_sel = NULL;
    if (!m_in.isEmpty())
    {
        m_sel = m_in.first();
        double max = m_sel->out();
        for(int i=1; i<m_in.size(); ++i) if (m_in[i]->out()>max)
        {
            m_sel = m_in[i];
            max   = m_sel->out();
        }
    }
}
void SScMinPoolNeuron::priv_poolselect()
{
    m_sel = NULL;
    if (!m_in.isEmpty())
    {
        m_sel = m_in.first();
        double min = m_sel->out();
        for(int i=1; i<m_in.size(); ++i) if (m_in[i]->out()<min)
        {
            m_sel = m_in[i];
            min   = m_sel->out();
        }
    }
}
void SScMedPoolNeuron::priv_poolselect()
{
    m_sel = NULL;

    if (!m_in.isEmpty())
    {
        QList<SScSortableNeuron> nl;
        nl.reserve(m_in.size());
        foreach(SSiHighwayNeuron* n, m_in) nl << n;
        std::sort(nl.begin(),nl.end());
        m_sel = nl[nl.size()/2].m_n;
    }
}
