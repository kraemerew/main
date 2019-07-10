#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"
#include "sschighwaynetwork.hpp"
#include "../nnhelpers/sscvm.hpp"

class SScCarryNeuron : public SSiHighwayNeuron
{
public:
    SScCarryNeuron(SScHighwayNetwork* net) : SSiHighwayNeuron(net, Carry), m_in(SScHighwayGate(this))
    {
        setActivation(SScActivation::LOGISTIC,1.0);
    }
    virtual double transform()
    {
        if (!m_transformset)
        {
            m_t = m_act->activate(net());
            m_transformset = true;
        }
        return m_t;
    }

    virtual double out()
    {
        return transform();
    }
    virtual bool addInput(SSiHighwayNeuron* other, SScTrainableParameter* tp)
    {
        SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
        if (cn) return false;
        return m_in.addInput(other,tp);
    }
    virtual bool addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
    {
        SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
        if (cn) return false;
        return m_in.addInput(other,v,t);
    }
    virtual bool delInput(SSiHighwayNeuron *other) { return m_in.delInput(other); }
    virtual double net() { return m_in.net(); }
    virtual void reset() { SSiHighwayNeuron::reset(); m_in.reset(); }
    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }
    virtual bool    connectHighway(SSiHighwayNeuron*, SSiHighwayNeuron*) { Q_ASSERT(false); return false; }
    virtual double  deltag()
    {
        return -dedo()*net()*m_act->dev();
    }
    virtual double deltaw(SSiHighwayNeuron* n)
    {
       return -dedo()*n->out()*m_act->dev()*m_act->gain();
    }

    virtual bool setActivation(SScActivation::Type type, double gain)
    {
        if (!SScActivation::canCarry(type)) return false;
        if (m_act) delete m_act;
        m_act = SScActivation::create(type);
        Q_CHECK_PTR(m_act);
        if (m_act) m_act->setGain(gain);
        return m_act!=NULL;
    }
    virtual QList<SSiHighwayNeuron*> inputs() const { return m_in.keys(); }
    virtual QList<SSiHighwayNeuron*> allInputs() const { return inputs(); }
    virtual double icon(SSiHighwayNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }


    virtual void trainingStep()
    {
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        {
            it.value()->update(deltaw(it.key()));
        }
        m_act->update(deltag());
    }
    virtual void endOfCycle()
    {
        m_in.endOfCycle();
        m_act->endOfCycle();        
    }
    virtual QVariantMap toVM() const
    {
        QVariantMap vm = SSiHighwayNeuron::toVM();
        vm["GATE"] = m_in.toVM();
        return vm;
    }
    virtual bool fromVM(const QVariantMap &vm)
    {
        bool ret = SSiHighwayNeuron::fromVM(vm);
        if (!m_in.fromVM(m_net,vm["GATE"].toMap())) ret = false;
        return ret;
    }

protected:
    virtual double priv_dedo()
    {
        double ret = 0;
        foreach(SSiHighwayNeuron* l, m_out) ret += l->dedo()*l->highwayMinusTransform();
        return ret;
    }
    SScHighwayGate m_in;
};

class SScConnectedNeuron : public SSiHighwayNeuron
{
public:
    SScConnectedNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype)
        : SSiHighwayNeuron(net,type,acttype),
          m_in(SScHighwayGate(this)),
          m_hwn(NULL), m_cn(NULL)
    {
    }
    virtual bool addInput(SSiHighwayNeuron* other, SScTrainableParameter* tp)
    {
        SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
        if (cn) return false;
        return m_in.addInput(other,tp);
    }
    virtual bool addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t)
    {
        SScCarryNeuron* cn = dynamic_cast<SScCarryNeuron*>(other);
        if (cn) return false;
        return m_in.addInput(other,v,t);
    }
    virtual bool delInput(SSiHighwayNeuron *other)
    {
        bool ret = false;
        if (other==m_cn) { m_cn=NULL; ret = true; }
        if (other==m_hwn) { m_hwn=NULL; ret = true; }
        if (m_in.delInput(other)) ret = true;
        return ret;
    }

    virtual double net() { return m_in.net(); }
    virtual QList<SSiHighwayNeuron*> inputs() const { return m_in.keys(); }
    virtual QList<SSiHighwayNeuron*> allInputs() const
    {
        QList<SSiHighwayNeuron*> ret = inputs();
        if (m_hwn) ret << m_hwn;
        if (m_cn) ret << m_cn;
        return ret;
    }
    virtual double icon(SSiHighwayNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }

    virtual void    reset()
    {
        m_dedoset=false; m_transformset=false; m_outset=false;
        SSiHighwayNeuron::reset();
        m_in.reset();
    }
    virtual double  carry() { return m_cn ? m_cn->out() : 0.0; }
    virtual double  highway() { return m_hwn ? m_hwn->out() : 0.0; }
    virtual bool connectHighway  (SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn)
    {
        SScCarryNeuron* dccn = dynamic_cast<SScCarryNeuron*>(cn);
        if ( (!hwn && !cn) || (hwn && cn && dccn && (hwn!=cn) && (hwn!=this) && (cn!=this)) )
        {
            m_hwn=hwn;
            m_cn=cn;
            return true;
        }
        return false;
    }
    virtual double transform()
    {
        if (!m_transformset)
        {
            m_t = m_act->activate(net());
            m_transformset = true;
        }
        return m_t;
    }

    virtual double out()
    {
       /* if (name()=="Out")
        {
            qWarning(">>>>T %s O %s", m_transformset ?"SET":"NOT SET", m_outset ? "SET":"NOT SET");
            qWarning(">>>>>>OUT %lf", transform()*(1.0-carry())+(carry()*highway()));
        }*/
        if (!m_outset)
        {            
            m_o = transform()*(1.0-carry())+(carry()*highway());
            m_outset = true;
        }
        return m_o;
    }
    virtual double  deltag()
    {
        return -dedo()*net()*m_act->dev()*(1.0-carry());
    }
    virtual double deltaw(SSiHighwayNeuron* n)
    {
        //TODO
        // dE/dw_ij=dE/doj*doj/dw_ij=dedo()*doj/dnetj*dnetj/dwij
        // = dedo()*doj/dnetj*oi
        return -dedo()*n->out()*m_act->dev()*m_act->gain()*(1.0-carry());
    }

    virtual void trainingStep()
    {
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        {
            it.value()->update(deltaw(it.key()));
        }
        m_act->update(deltag());
    }
    virtual void endOfCycle()
    {
        m_in.endOfCycle();
        m_act->endOfCycle();        
    }
    virtual QVariantMap toVM() const
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
    virtual bool fromVM(const QVariantMap &vm)
    {
        bool ret = SSiHighwayNeuron::fromVM(vm);
        if (!m_in.fromVM(m_net,vm["GATE"].toMap())) ret = false;
        if (vm.contains("HIGHWAY") && vm.contains("CARRY"))
        {
            bool ok1 = false, ok2 = false;
            const int hwidx = vm["HIGHWAY"].toInt(&ok1),
                      cidx  = vm["CARRY"].  toInt(&ok2);
            if (ok1 && ok2 && (hwidx>=0) && (cidx>=0) && (hwidx!=cidx))
            {
                m_net->setHighway(index(),hwidx,cidx);
            }
            else ret = false;
        }
        return ret;
    }
    SScHighwayGate      m_in;
    SSiHighwayNeuron*   m_hwn;
    SSiHighwayNeuron*   m_cn;
};

class SScHiddenNeuron : public SScConnectedNeuron
{
public:
    SScHiddenNeuron(SScHighwayNetwork* net) : SScConnectedNeuron(net,Hidden, SScActivation::MHAT)
    {}

    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

};

class SScInputNeuron : public SSiHighwayNeuron
{
public:
    SScInputNeuron(SScHighwayNetwork* net) : SSiHighwayNeuron(net,Input) {}
    bool addInput(SSiHighwayNeuron *, double, SScTrainableParameter::Type) { return false; }
    bool addInput(SSiHighwayNeuron*, SScTrainableParameter*) { return false; }

    bool delInput(SSiHighwayNeuron *) { return false; }

    virtual bool    setInput    (double v)          { m_input = v; return true; }
    virtual bool    setTarget   (double)            { Q_ASSERT(false); return false; }
    virtual double  deltag      ()                  { return 0.0; }
    virtual double  deltaw      (SSiHighwayNeuron*) { return 0; }
    virtual double  net         ()                  { return m_input; }
    virtual double  out         ()                  { return net(); }
    virtual double  carry       ()                  { return 0.0; }
    virtual double  highway     ()                  { return 0.0; }
    virtual bool    connectHighway(SSiHighwayNeuron*, SSiHighwayNeuron*) { Q_ASSERT(false); return false; }

private:
    double                  m_input;
};

class SScBiasNeuron : public SSiHighwayNeuron
{
public:
    SScBiasNeuron(SScHighwayNetwork* net) : SSiHighwayNeuron(net,Bias) {}
    virtual bool    addInput    (SSiHighwayNeuron*, double,SScTrainableParameter::Type)  { Q_ASSERT(false); return false; }
    virtual bool    addInput    (SSiHighwayNeuron*, SScTrainableParameter*) { return false; }
    virtual bool    delInput    (SSiHighwayNeuron*)           { Q_ASSERT(false); return false; }
    virtual bool    setInput    (double)                { Q_ASSERT(false); return false; }
    virtual bool    setTarget   (double)                { Q_ASSERT(false); return false; }
    virtual double  deltag      ()                      { return 0.0; }
    virtual double  deltaw      (SSiHighwayNeuron*)            { return 0.0; }
    virtual double  net         ()                      { return 1.0; }
    virtual double  out         ()                  { return 1.0; }
    virtual double  carry       ()                  { return 0.0; }
    virtual double  highway     ()                  { return 0.0; }
    virtual bool connectHighway(SSiHighwayNeuron*, SSiHighwayNeuron*) { Q_ASSERT(false); return false; }
};

class SScOutputNeuron : public SScConnectedNeuron
{
public:
    SScOutputNeuron(SScHighwayNetwork* net)
        : SScConnectedNeuron(net,Output,SScActivation::SWISH)
    {}

    virtual bool    setInput(double) { Q_ASSERT(false); return false; }
    virtual bool    setTarget(double v) { m_target = v; return true; }
    virtual double  err() { return out()-m_target; }
    virtual void    connectForward(const QList<SSiHighwayNeuron*>&) { }

private:

    virtual double priv_dedo()
    {
        // Output error derivative by this output
        // d(out-target)^2/(d out) = 2(out-target)
        return 2.0*err();
    }

    double m_target;
};

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
       const double w_jl = l->icon(this);
       ret += w_jl*l->dedo()*l->act()->dev()*l->act()->gain()*(1.0-l->carry());
    }
    return ret;
}

int SSiHighwayNeuron::index() { return m_net->n2idx(this); }

QVariantMap SSiHighwayNeuron::toVM() const
{
    QVariantMap vm;
    vm["TYPE"] = type2Id(m_type);
    vm["ACT"] = m_act->toVM();
    return vm;
}

bool SSiHighwayNeuron::fromVM(const QVariantMap & vm)
{
    foreach(const QString& key, vm.keys())
    {
        if (key=="ACT")
        {
            if (m_act) delete m_act;
            m_act = SScActivation::create(vm[key].toMap());
        }
    }
    return true;
}

QString SSiHighwayNeuron::type2Id(Type t)
{
    switch(t)
    {
    case Input:   return "INPUT";   break;
    case Hidden:  return "HIDDEN";  break;
    case Output:  return "OUTPUT";  break;
    case Bias:    return "BIAS";    break;
    case Carry:   return "CARRY";   break;
    default:                        break;
    }
    return "";
}

SSiHighwayNeuron::Type SSiHighwayNeuron::id2Type(const QString &id)
{
    for (int i= (int)Input; i<(int)Last; ++i)
        if (id.toUpper()==type2Id((Type)i)) return (Type)i;
    return Last;
}
