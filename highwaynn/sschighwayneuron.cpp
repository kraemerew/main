#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"
#include <future>
#include <thread>

double multiply(double* d, unsigned int len)
{
    double ret = 1.0;
    for (unsigned int i = 0; i<len; ++i) ret*=d[i];
    return ret;
}

class SScCarryNeuron : public SSiHighwayNeuron
{
public:
    SScCarryNeuron() : SSiHighwayNeuron(NeuronType_Carry), m_in(SScHighwayGate(this))
    {
        setActivation(SScActivation::ACT_SIGMOID,1.0);
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
    virtual bool addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t) { return m_in.addInput(other,v,t); }
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
    SScConnectedNeuron(SSeNeuronType type, SScActivation::Type acttype)
        : SSiHighwayNeuron(type,acttype),
          m_in(SScHighwayGate(this)),
          m_hwn(NULL), m_cn(NULL)
    {
    }
    virtual bool addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t) { return m_in.addInput(other,v,t); }
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

    virtual void    reset() { SSiHighwayNeuron::reset(); m_in.reset(); }
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

    SScHighwayGate      m_in;
    SSiHighwayNeuron*   m_hwn;
    SSiHighwayNeuron*   m_cn;
};

class SScHiddenNeuron : public SScConnectedNeuron
{
public:
    SScHiddenNeuron() : SScConnectedNeuron(NeuronType_Hidden, SScActivation::ACT_TANH)
    {}

    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

};

class SScInputNeuron : public SSiHighwayNeuron
{
public:
    SScInputNeuron() : SSiHighwayNeuron(NeuronType_Input) {}
    bool addInput(SSiHighwayNeuron *, double, SScTrainableParameter::Type) { return false; }
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
    SScBiasNeuron() : SSiHighwayNeuron(NeuronType_Bias) {}
    virtual bool    addInput    (SSiHighwayNeuron*, double,SScTrainableParameter::Type)  { Q_ASSERT(false); return false; }
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
    SScOutputNeuron() : SScConnectedNeuron(NeuronType_Output,SScActivation::ACT_MHAT)
    {        
    }

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

SSiHighwayNeuron* SSiHighwayNeuron::create(SSeNeuronType type, const QString& name)
{
    SSiHighwayNeuron* ret = NULL;
    switch(type)
    {
        case NeuronType_Hidden: ret = new (std::nothrow) SScHiddenNeuron (); break;
        case NeuronType_Input:  ret = new (std::nothrow) SScInputNeuron  (); break;
        case NeuronType_Output: ret = new (std::nothrow) SScOutputNeuron (); break;
        case NeuronType_Bias:   ret = new (std::nothrow) SScBiasNeuron   (); break;
        case NeuronType_Carry:  ret = new (std::nothrow) SScCarryNeuron  (); break;
    }
    ret->setName(name);
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

    /*std::vector<std::future<double> > fv;
    fv.reserve(m_out.size());
    foreach(SSiHighwayNeuron* l, m_out)
    {
        double d[] = { l->dedo(), l->icon(this), l->act()->dev(),l->act()->gain(), (1.0-l->carry()) };
        fv.insert(fv.end(),std::async(multiply,d,4));
    }
    for (auto it = fv.begin(); it!=fv.end(); ++it) ret += it->get();
*/
    return ret;
}
