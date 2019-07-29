#include "gated.hpp"
#include "carry.hpp"
#include "sscvm.hpp"
#include "network.hpp"

SScGatedNeuron::SScGatedNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype)
    : SSiHighwayNeuron  (net,type,acttype),
      m_in              (SScGate(this)),
      m_hwn             (NULL),
      m_cn              (NULL)
{}
bool SScGatedNeuron::addConnection(SSiHighwayNeuron* other, SScTrainableParameter* tp)
{
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    if (cn) return false;
    const bool ret = m_in.addInput(other,tp);
    if (ret) other->addFwdConnection(this);
    return ret;
}
bool SScGatedNeuron::addConnection(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
{
    SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
    if (cn) return false;
    const bool ret = m_in.addInput(other,v,t);
    if (ret) other->addFwdConnection(this);
    return ret;
}
bool SScGatedNeuron::delConnection(SSiHighwayNeuron *other)
{
    bool ret = false;
    if (other==m_cn)    { m_cn  = NULL; ret = true; }
    if (other==m_hwn)   { m_hwn = NULL; ret = true; }
    if (m_in.delInput(other))
    {
        other->delFwdConnection(this);
        ret = true;
    }
    return ret;
}

QList<SSiHighwayNeuron*> SScGatedNeuron::inputs() const { return m_in.keys(); }

QList<SSiHighwayNeuron*> SScGatedNeuron::allInputs() const
{
    QList<SSiHighwayNeuron*> ret = inputs();
    if (m_hwn) ret << m_hwn;
    if (m_cn) ret << m_cn;
    return ret;
}
double SScGatedNeuron::icon(SSiHighwayNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }

void SScGatedNeuron::reset()
{
    m_dedoset=false; m_transformset=false; m_outset=false;
    SSiHighwayNeuron::reset();
    m_in.reset();
}
double  SScGatedNeuron::carry() { return m_cn ? m_cn->out() : 0.0; }
double  SScGatedNeuron::highway() { return m_hwn ? m_hwn->out() : 0.0; }
bool SScGatedNeuron::connectHighway  (SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn)
{
    SScCarryNeuron* dccn = dynamic_cast<SScCarryNeuron*>(cn);
    if ( (!hwn && !cn) || (hwn && cn && dccn && (hwn!=cn) && (hwn!=this) && (cn!=this)) )
    {
        if (m_cn) m_cn->delFwdConnection(this);
        m_hwn=hwn;
        m_cn=cn;
        if (cn) cn->addFwdConnection(this);
        return true;
    }
    return false;
}
double SScGatedNeuron::transform()
{
    if (!m_transformset)
    {
        m_t = m_act->activate(net());
        m_transformset = true;
    }
    return m_t;
}

double SScGatedNeuron::net() { return m_in.net(); }
double SScGatedNeuron::out()
{
    if (!m_outset)
    {
        m_o = (transform()*(1.0-carry()))+(carry()*highway());
        m_outset = true;
     }
    return m_o;
}

double SScGatedNeuron::deltaw(SSiHighwayNeuron* n)
{
    // dE/dw_ij=dE/doj*doj/dw_ij=dedo()*doj/dnetj*dnetj/dwij
    // = dedo()*doj/dnetj*oi
    return -dedo()*n->out()*m_act->dev()*m_act->gain()*(1.0-carry());
}

void SScGatedNeuron::trainingStep()
{
    if (!m_conlock) for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        it.value()->update(deltaw(it.key()));    
    if (!m_gainlock) m_act->update(dedo(),carry());
}
void SScGatedNeuron::endOfCycle()
{
    if (!m_conlock)  m_in.  endOfCycle();
    if (!m_gainlock) m_act->endOfCycle();
}
QVariantMap SScGatedNeuron::toVM() const
{
    QVariantMap vm = SSiHighwayNeuron::toVM();
    vm["GATE"] = m_in.toVM();
    if (m_hwn && m_cn)
    {
        vm["HIGHWAY"] = m_hwn->index();
        vm["CARRY"]   = m_cn ->index();
    }
    return vm;
}
bool SScGatedNeuron::fromVM(const QVariantMap &vm)
{
    SScVM sscvm(vm);
    bool ret = SSiHighwayNeuron::fromVM(vm);
    m_in.clear();
    if (!m_in.fromVM(m_net,sscvm.vmToken("GATE"))) ret = false;
    const int hwidx = sscvm.intToken("HIGHWAY",-1),
              cidx  = sscvm.intToken("CARRY",  -1);
    if ((hwidx>=0) && (cidx>=0))
    {
        if (hwidx!=cidx) m_net->setHighway(index(),hwidx,cidx);
        else ret = false;
    }
    return ret;
}

void SScGatedNeuron::dump()
{
    SSiHighwayNeuron::dump();
    qWarning("> %d connections", m_in.size());
    if (m_hwn && m_cn) qWarning("> Highway connected");
}





