#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


class SScConnectedNeuron : public SSiHighwayNeuron
{
public:
    SScConnectedNeuron(SSeNeuronType type, SScActivation::Type acttype)
        : SSiHighwayNeuron(type,acttype),
          m_in(SScHighwayGate(this)),
          m_hwn(NULL), m_cn(NULL)
    {
    }
    virtual bool addInput(SSiHighwayNeuron *other, double v) { return m_in.addInput(other,v); }
    virtual bool delInput(SSiHighwayNeuron *other) { return m_in.delInput(other); }
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

    virtual void    reset() { m_outset = false; m_dedoset=false; m_in.reset(); }
    virtual double  carry() { return m_cn ? m_cn->out() : 0.0; }
    virtual double  highway() { return m_hwn ? m_hwn->out() : 0.0; }
    virtual bool connectHighway  (SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn)
    {
        if ( (!hwn && !cn) || (hwn && cn && (hwn!=cn) && (hwn!=this) && (cn!=this)) )
        {
            m_hwn=hwn;
            m_cn=cn;
            return true;
        }
        return false;
    }

    virtual double out()
    {
        if (!m_outset)
        {
            m_o = m_act->activate(net())*(1.0-carry())+carry()*highway();
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
            it.value()->update(deltaw(it.key()));
        m_act->update(deltag());
    }
    virtual void endOfCycle()
    {
        m_in.endOfCycle();
        m_act->endOfCycle();
        reset();
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
    bool addInput(SSiHighwayNeuron *, double ) { return false; }
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
    virtual bool    addInput    (SSiHighwayNeuron*, double)  { Q_ASSERT(false); return false; }
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
    virtual double  out() { return m_act->activate(net()); }
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

bool SSiHighwayNeuron::setActivation(SScActivation::Type type)
{
    if (m_act) delete m_act;
    m_act = SScActivation::create(type);
    Q_CHECK_PTR(m_act);
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
    }
    ret->setName(name);
    return ret;
}



