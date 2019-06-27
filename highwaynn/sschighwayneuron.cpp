#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


class SScConnectedNeuron : public SSiHighwayNeuron
{
public:
    SScConnectedNeuron(SSeNeuronType type, SScActivation::Type acttype) : SSiHighwayNeuron(type,acttype), m_in(SScHighwayGate(this))
    {
    }
    virtual bool addInput(SSiHighwayNeuron *other, double v) { return m_in.addInput(other,v); }
    virtual bool delInput(SSiHighwayNeuron *other) { return m_in.delInput(other); }
    virtual double net() { return m_in.net(); }
    virtual QList<SSiHighwayNeuron*> inputs() const { return m_in.keys(); }
    virtual double icon(SSiHighwayNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }

    virtual double deltag() { return -dedo()*net()*m_act->dev(); }
    virtual void reset() { m_outset = false; m_dedoset=false; m_in.reset(); }
    virtual double out()
    {
        //if (!m_outset)
        {
            m_o = m_act->activate(net());
            m_outset = true;
        }
        return m_o;
    }
    virtual double deltaw(SSiHighwayNeuron* n)
    {
        //TODO
        // dE/dw_ij=dE/doj*doj/dw_ij=dedo()*doj/dnetj*dnetj/dwij
        // = dedo()*doj/dnetj*oi

        return -dedo()*n->out()*m_act->dev()*m_act->gain();
    }

    virtual void trainingStep()
    {
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
            it.value()->update(deltaw(it.key()));
        m_act->update(deltag());
    }
    virtual void endOfCycle()
    {
        for(QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
            it.value()->endOfCycle();
        m_act->endOfCycle();
    }

    SScHighwayGate m_in;
};

class SScHiddenNeuron : public SScConnectedNeuron
{
public:
    SScHiddenNeuron() : SScConnectedNeuron(NeuronType_Hidden, SScActivation::ACT_TANH)
    {
    }

    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    //virtual double out() { return m_act->activate(net()); }
};



class SScInputNeuron : public SSiHighwayNeuron
{
public:
    SScInputNeuron() : SSiHighwayNeuron(NeuronType_Input) {}
    bool addInput(SSiHighwayNeuron *, double ) { return false; }
    bool delInput(SSiHighwayNeuron *) { return false; }

    virtual bool  setInput(double v) { m_input = v; return true; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    virtual double deltag() { return 0.0; }
    virtual double deltaw(SSiHighwayNeuron*) { return 0; }
    virtual double net() { return m_input; }
    virtual double out() { return net(); }
    virtual QList<SSiHighwayNeuron*> inputs() const { return QList<SSiHighwayNeuron*>();  }

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
    virtual double  out         ()                      { return 1.0; }
    virtual QList<SSiHighwayNeuron*> inputs() const { return QList<SSiHighwayNeuron*>();  }

private:
    //QList<SSiHighwayNeuron*>       m_out;
};

class SScOutputNeuron : public SScConnectedNeuron
{
public:
    SScOutputNeuron() : SScConnectedNeuron(NeuronType_Output,SScActivation::ACT_MHAT)
    {
        //setActivation(SScActivation::ACT_SIGMOID);
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


class SScHighwayNeuron : public SScConnectedNeuron
{
public:
    explicit SScHighwayNeuron()
        : SScConnectedNeuron(NeuronType_Highway,SScActivation::ACT_SWISH),
          m_cin     (this),
          m_cact    (SScActivation::create(SScActivation::ACT_SIGMOID)),
          m_hw      (NULL)
    {

    }

    virtual ~SScHighwayNeuron()
    {
        if (m_act) delete m_act;
        m_act=NULL;
        delete m_cact;
    }

    virtual void    reset() { m_dedoset=false; m_outset=false; m_in.reset(); m_cin.reset(); }

    virtual double deltaw(SSiHighwayNeuron*) { return 0; } //TODO
    virtual double deltag() { return 0; } // TODO

    virtual bool    addInputC(SSiHighwayNeuron* other, double v) { return m_cin.addInput(other,v); }
    virtual bool    delInputC(SSiHighwayNeuron* other) { return m_cin.delInput(other); }
    virtual double  netC() { return m_cin.net(); }
    virtual double deltawC(SSiHighwayNeuron*) { return 0; } //TODO
    virtual double deltagC() { return 0; } // TODO
    virtual double outC() { return m_cact->activate(netC()); }
    virtual double highway() { return m_hw ? m_hw->out() : 0.0; }
    virtual void setHighway(SSiHighwayNeuron* hw) { m_hw=hw; }
    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }
    virtual double out()
    {
        //if (!m_outset)
        {
            const double c = outC();
            m_o= m_act->activate(net())*(1.0-c)+c*highway();
            m_outset=true;
        }
        return m_o;
    }

    virtual QList<SSiHighwayNeuron*> inputsC() const { return m_cin.keys(); }


protected:
    SScHighwayGate      m_cin;
    SScActivation*      m_cact;
    SSiHighwayNeuron*   m_hw;
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
        case NeuronType_Highway:ret = new (std::nothrow) SScHighwayNeuron(); break;
    }
    ret->setName(name);
    return ret;
}



