#include "neuron.hpp"
#include "network.hpp"
#include "network.hpp"
#include "../nnhelpers/sscvm.hpp"
#include "bias.hpp"
#include "input.hpp"
#include "carry.hpp"
#include "gated.hpp"
#include "pool.hpp"

SSiHighwayNeuron::SSiHighwayNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype)
    : m_net(net),
      m_type(type),
      m_conlock(false),
      m_gainlock(false),
      m_dedoset(false),
      m_transformset(false),
      m_outset(false),
      m_dedo(0.0),
      m_t(0.0),
      m_o(0.0),
      m_act(SScActivation::create(acttype))
{
   Q_CHECK_PTR(net);
   reset();
}

SSiHighwayNeuron::~SSiHighwayNeuron()
{
    if (m_act) delete m_act;
    m_act=NULL;
}

bool SSiHighwayNeuron::setActivation(SScActivation::Type type, double gain)
{
    if (m_act) delete m_act;
    m_act = SScActivation::create(type);
    Q_CHECK_PTR(m_act);
    if (m_act) m_act->setGain(gain);
    return m_act!=NULL;
}

SSiHighwayNeuron* SSiHighwayNeuron::create(SScHighwayNetwork* net, const QVariantMap& vm)
{
    SScVM sscvm(vm);
    const Type t = id2Type(sscvm.stringToken("TYPE",""));
    if (t!=SSiHighwayNeuron::Last)
    {
        SSiHighwayNeuron* n = create(net,t,"");
        return n;
    }
    return NULL;
}

SSiHighwayNeuron* SSiHighwayNeuron::create(SScHighwayNetwork* net, Type type, const QString& name)
{
    SSiHighwayNeuron* ret = NULL;
    switch(type)
    {
        case Hidden: ret = new (std::nothrow) SScHiddenNeuron (net); break;
        case Input:  ret = new (std::nothrow) SScInputNeuron  (net); break;
        case Output: ret = new (std::nothrow) SScOutputNeuron (net); break;
        case Bias:   ret = new (std::nothrow) SScBiasNeuron   (net); break;
        case Carry:  ret = new (std::nothrow) SScCarryNeuron  (net); break;
        case MinPool:ret = new (std::nothrow) SScMinPoolNeuron(net); break;
        case MaxPool:ret = new (std::nothrow) SScMaxPoolNeuron(net); break;
        case MedPool:ret = new (std::nothrow) SScMedPoolNeuron(net); break;
        default:                                                     break;
    }
    Q_CHECK_PTR(ret);
    if (ret) ret->setName(name);
    return ret;
}

double SSiHighwayNeuron::priv_dedo()
{
    // dE/do_j = sum(l) dE/dnet_l * dnet_l/do_j = sum(l) dE/dnet_l wjl
    //         = sum(l) w_jl dE/do_l do_l/dnet_l
    //         = sum(l) w_jl dedo(l) act(l)'gain_l
    double ret = 0;

    foreach(SSiHighwayNeuron* l, m_out)
    {
        const double seldedo = l->forwardSelectedDedo(this);
        //if (seldedo>0) qWarning(">>>>>>FWDDEDO %lf %s->%s", seldedo, qPrintable(l->name()), qPrintable(name()));
        ret += seldedo;    // only the pool neuron delivers something here, if this neuron achieved maximum in pool
        const double w_jl = l->icon(this);
        ret +=  w_jl*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
    }
    return ret;
}

int SSiHighwayNeuron::index() { return m_net->n2idx(this); }

QVariantMap SSiHighwayNeuron::toVM() const
{
    QVariantMap vm;
    vm["TYPE"] = type2Id(m_type);
    vm["NAME"] = m_name;
    vm["ACT"]  = m_act->toVM();
    if (m_conlock)  vm["TLOCK_CON"]  = m_conlock;
    if (m_gainlock) vm["TLOCK_GAIN"] = m_gainlock;
    return vm;
}

bool SSiHighwayNeuron::fromVM(const QVariantMap & vm)
{
    SScVM sscvm(vm);
    m_name = sscvm.stringToken("NAME","");
    const QVariantMap avm = sscvm.vmToken("ACT");
    if (!avm.isEmpty())
    {
        if (m_act) delete m_act;
        m_act = SScActivation::create(avm);        
    }
    setConLock (sscvm.boolToken("TLOCK_CON", false));
    setGainLock(sscvm.boolToken("TLOCK_GAIN",false));

    return true;
}

QString SSiHighwayNeuron::type2Id(Type t)
{
    switch(t)
    {
    case Input:   return "INPUT";       break;
    case Hidden:  return "HIDDEN";      break;
    case Output:  return "OUTPUT";      break;
    case Bias:    return "BIAS";        break;
    case Carry:   return "CARRY";       break;
    case MaxPool: return "MAXPOOL";     break;
    case MinPool: return "MINPOOL";     break;
    case MedPool: return "MEDIANPOOL";  break;
    default:                            break;
    }
    return "";
}

SSiHighwayNeuron::Type SSiHighwayNeuron::id2Type(const QString &id)
{
    for (int i= (int)Input; i<(int)Last; ++i)
        if (id.toUpper()==type2Id((Type)i)) return (Type)i;
    return Last;
}

void SSiHighwayNeuron::dump()
{
    if (m_name.isEmpty())   qWarning("> Type %s", qPrintable(type2Id(m_type)));
    else                    qWarning("> Type %s / Name %s", qPrintable(type2Id(m_type)), qPrintable(m_name));
}
