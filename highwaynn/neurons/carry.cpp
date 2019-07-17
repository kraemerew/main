#include "carry.hpp"
#include "sscvm.hpp"
#include "gate.hpp"

SScCarryNeuron::SScCarryNeuron(SScHighwayNetwork* net)
    : SSiHighwayNeuron(net, Carry),
      m_in(SScGate(this))
{
    setActivation(SScActivation::LOGISTIC,1.0);
}
double SScCarryNeuron::transform()
{
    if (!m_transformset)
    {
        m_t = m_act->activate(net());
        m_transformset = true;
    }
    return m_t;
}

bool SScCarryNeuron::addInput(SSiHighwayNeuron* other, SScTrainableParameter* tp)
{
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    if (cn) return false;
    return m_in.addInput(other,tp);
}
bool SScCarryNeuron::addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
{
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    if (cn) return false;
    return m_in.addInput(other,v,t);
}
bool    SScCarryNeuron::delInput        (SSiHighwayNeuron *other) { return m_in.delInput(other); }
double  SScCarryNeuron::net             () { return m_in.net(); }
void    SScCarryNeuron::reset           () { SSiHighwayNeuron::reset(); m_in.reset(); }
double  SScCarryNeuron::deltaw          (SSiHighwayNeuron* n) { return -dedo()*n->out()*m_act->dev()*m_act->gain(); }

bool SScCarryNeuron::setActivation (SScActivation::Type type, double gain)
{
    if (!SScActivation::canCarry(type)) return false;
    if (m_act) delete m_act;
    m_act = SScActivation::create(type);
    Q_CHECK_PTR(m_act);
    if (m_act) m_act->setGain(gain);
    return m_act!=NULL;
}

void SScCarryNeuron::trainingStep()
{
    if (!m_conlock) for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        it.value()->update(deltaw(it.key()));    
    if (!m_gainlock) m_act->update(dedo(),0.0);
}
void SScCarryNeuron::endOfCycle()
{
    if (!m_conlock)  m_in.  endOfCycle();
    if (!m_gainlock) m_act->endOfCycle();
}
QVariantMap SScCarryNeuron::toVM() const
{
    QVariantMap vm = SSiHighwayNeuron::toVM();
    vm["GATE"] = m_in.toVM();
    return vm;
}
bool SScCarryNeuron::fromVM(const QVariantMap &vm)
{
    bool ret = SSiHighwayNeuron::fromVM(vm);
    m_in.clear();
    SScVM sscvm(vm);
    if (!m_in.fromVM(m_net,sscvm.vmToken("GATE"))) ret = false;
    return ret;
}

void SScCarryNeuron::dump()
{
    SSiHighwayNeuron::dump();
    qWarning("> %d connections", m_in.size());
}

double SScCarryNeuron::priv_dedo()
{
    double ret = 0;
    foreach(SSiHighwayNeuron* l, m_out) ret += l->dedo()*l->highwayMinusTransform();
    return ret;
}
