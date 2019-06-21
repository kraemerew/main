#include "sscneuron.hpp"
#include "QSharedPointer"
#include <QtMath>
#include <QMap>


class SScConnectionRProp : public SScConnection
{
public:
    SScConnectionRProp(double v) : SScConnection(v), m_uval(.1), m_last(0) {}
    virtual void update(double v, bool cycleDone)
    {
        m_updatesum += v;
        if (cycleDone)
        {
            const bool signchange = ((m_updatesum>0) && (m_last<0)) || ((m_updatesum<0) && (m_last>0));
            if (signchange) m_uval*=.6; else m_uval*=1.2;
            m_uval=qBound(0.0000001,m_uval,100.0);
            //qWarning(">>>>>>>>>>>RPROP UPDATE %s DLT %lf, V %lf", signchange ?"SC":"SS",m_uval,m_value);
            if (m_updatesum>0) m_value+=m_uval; else if (m_updatesum<0) m_value-=m_uval;
            m_last=m_updatesum;
            m_updatesum=0.0;
        }
    }
private:
    double m_uval, m_last;
};

SScConnection* SScConnection::create(SSeConnection type, double v)
{
    switch(type)
    {
    case CON_STD: return new SScConnection(v); break;
    case CON_RPROP: return new SScConnectionRProp(v); break;
    }
    return NULL;
}

class SScActivationIdentity : public SScActivation
{
public:
    SScActivationIdentity(): SScActivation() {}
    virtual QString name() const { return "Identity"; }

private:
    virtual void priv_activate() { m_act = m_pot*m_gain->value(); }
    virtual double priv_dev() { return m_gain->value(); }
};
class SScActivationSigmoid : public SScActivation
{
public:
    SScActivationSigmoid() : SScActivation() {}
    virtual QString name() const { return "Logistic"; }
private:
    virtual void priv_activate() { m_act = 1.0/(1.0+exp(-m_pot*m_gain->value())); }
    virtual double priv_dev() { return m_act*(1.0-m_act); }
};

class SScActivationTanh : public SScActivation
{
public:
    SScActivationTanh(): SScActivation() {}
    virtual QString name() const { return "Tanh"; }

private:
    virtual void priv_activate() { m_act = tanh(m_pot*m_gain->value()); }
    virtual double priv_dev() { return 1.0-qPow(m_act,2.0); }
};

class SScActivationRbf: public SScActivation
{
public:
    SScActivationRbf() : SScActivation() {}
    virtual QString name() const { return "Rbf"; }
private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_act = exp(-qPow(m_gp,2.0)); }
    virtual double priv_dev() { return -2.0*m_gp*m_act; }
    double m_gp;
};


class SScActivationMHat: public SScActivation
{
public:
    SScActivationMHat() : SScActivation() {}
    virtual QString name() const { return "Rbf"; }
private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_gp2 = qPow(m_gp,2.0); m_act = (1.0-m_gp2)*exp(-m_gp2); }
    virtual double priv_dev() { return (m_gp==1.0) ? -2.0/exp(1) : -2.0*m_gp*m_act*(1+1.0/(1.0-m_gp2)); }
    double m_gp, m_gp2;
};

class SScActivationGaussianDerivative : public SScActivation
{
public:
    SScActivationGaussianDerivative() : SScActivation() {}
    virtual QString name() const { return "GDer"; }
private:
    virtual void priv_activate() { m_x = m_pot*m_gain->value(); m_act = -2.0*m_x*exp(-qPow(m_x,2.0)); }
    virtual double priv_dev() { return (m_x!=0.0) ? ((1.0/m_x)-(2.0*m_x)) * m_act : -2.0; }
    double m_x;
};

class SScActivationSoftPlus : public SScActivation
{
public:
    SScActivationSoftPlus() : SScActivation() {}
    virtual QString name() const { return "SoftPlus"; }
private:
    virtual void priv_activate() { m_e = exp(m_pot*m_gain->value()); m_act = log(1+m_e); }
    virtual double priv_dev() { return m_e/(1.0+m_e); }
    double m_e;
};

SScActivation* SScActivation::create(SSeActivation type)
{
    switch (type)
    {
    case ACT_IDENTITY:  return new SScActivationIdentity(); break;
    case ACT_SIGMOID:   return new SScActivationSigmoid(); break;
    case ACT_TANH:      return new SScActivationTanh(); break;
    case ACT_RBF:       return new SScActivationRbf(); break;
    case ACT_SOFTPLUS:  return new SScActivationSoftPlus(); break;
    case ACT_MHAT:      return new SScActivationMHat(); break;
    case ACT_GDER:      return new SScActivationGaussianDerivative(); break;
    }
    return NULL;
}



class SScConnectedNeuron : public SScNeuron
{
public:
    SScConnectedNeuron(SSeNeuronType type) : SScNeuron(type)
    {
    }
    virtual bool addInput(SScNeuron *other, double v)
    {
        Q_CHECK_PTR(other);
        if ((this==other) || m_in.contains(other)) return false;
        m_in[other]=QSharedPointer<SScConnection>(SScConnection::create(SScConnection::CON_RPROP,v));
        return true;
    }
    virtual bool delInput(SScNeuron *other)
    {
        Q_CHECK_PTR(other);
        if (!m_in.contains(other)) return false;
        m_in.remove(other);
        return true;
    }
    virtual double net()
    {
        double ret = 0;
        for(QMap<SScNeuron*,QSharedPointer<SScConnection> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
            ret += it.key()->out()*it.value()->value();
        return ret;
    }
    virtual double deltag()
    {
        return -dedo()*net()*m_act->dev();
    }

    virtual double deltaw(SScNeuron* n)
    {
        //TODO
        // dE/dw_ij=dE/doj*doj/dw_ij=dedo()*doj/dnetj*dnetj/dwij
        // = dedo()*doj/dnetj*oi

        return -dedo()*n->out()*m_act->dev()*m_act->gain();
    }
    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }

    virtual double icon(SScNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }

    virtual bool trainingStep(bool cycleDone)
    {                
        for(QMap<SScNeuron*,QSharedPointer<SScConnection> >::iterator it = m_in.begin(); it != m_in.end(); ++it)        
            it.value()->update(deltaw(it.key()),cycleDone);
        m_act->updateGain(deltag(),cycleDone);
        return true;
    }

    QMap<SScNeuron*,QSharedPointer<SScConnection> > m_in;
};

class SScHiddenNeuron : public SScConnectedNeuron
{
public:
    SScHiddenNeuron() : SScConnectedNeuron(NeuronType_Hidden)
    {
    }

    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    virtual double out() { return m_act->activate(net()); }
};



class SScInputNeuron : public SScNeuron
{
public:
    SScInputNeuron() : SScNeuron(NeuronType_Input) {}
    bool addInput(SScNeuron *, double ) { return false; }
    bool delInput(SScNeuron *) { return false; }

    virtual bool  setInput(double v) { m_input = v; return true; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    virtual double deltag() { return 0.0; }
    virtual double deltaw(SScNeuron*) { return 0; }
    virtual double net() { return m_input; }
    virtual double out() { return net(); }
    virtual QList<SScNeuron*> inputs() const { return QList<SScNeuron*>();  }
    virtual bool trainingStep(bool) { return false; }

private:
    double                  m_input;
};

class SScBiasNeuron : public SScNeuron
{
public:
    SScBiasNeuron() : SScNeuron(NeuronType_Bias) {}
    virtual bool    addInput    (SScNeuron *, double )  { Q_ASSERT(false); return false; }
    virtual bool    delInput    (SScNeuron *)           { Q_ASSERT(false); return false; }
    virtual bool    setInput    (double)                { Q_ASSERT(false); return false; }
    virtual bool    setTarget   (double)                { Q_ASSERT(false); return false; }
    virtual double  deltag      ()                      { return 0.0; }
    virtual double  deltaw      (SScNeuron*)            { return 0.0; }
    virtual double  net         ()                      { return 1.0; }
    virtual double  out         ()                      { return 1.0; }
    virtual QList<SScNeuron*> inputs() const { return QList<SScNeuron*>();  }
    virtual bool trainingStep(bool) { return false; }

private:
    QList<SScNeuron*>       m_out;
};

class SScOutputNeuron : public SScConnectedNeuron
{
public:
    SScOutputNeuron() : SScConnectedNeuron(NeuronType_Output)
    {
        setActivation(SScActivation::ACT_SIGMOID);
    }

    virtual bool    setInput(double) { Q_ASSERT(false); return false; }
    virtual bool    setTarget(double v) { m_target = v; return true; }
    virtual double  err() { return out()-m_target; }
    virtual double  out() { return m_act->activate(net()); }
    virtual void    connectForward(const QList<SScNeuron*>&) { }

private:

    virtual double priv_dedo()
    {        
        // Output error derivative by this output
        // d(out-target)^2/(d out) = 2(out-target)
        return 2.0*err();
    }

    double                  m_target;
};

SScNeuron::SScNeuron(SSeNeuronType type)
    : m_type    (type),
      m_dedoset (false),
      m_dedo    (0.0),
      m_act     (NULL)
{
    switch(type)
    {
        case NeuronType_Hidden: setActivation(SScActivation::ACT_TANH);     break;
        case NeuronType_Output: setActivation(SScActivation::ACT_SIGMOID); break;
        default:                setActivation(SScActivation::ACT_IDENTITY); break;
    }
}

bool SScNeuron::setActivation(SScActivation::SSeActivation type)
{
    if (m_act) delete m_act;
    m_act = SScActivation::create(type);
    Q_CHECK_PTR(m_act);
    return m_act!=NULL;
}

SScNeuron::~SScNeuron() {}

SScNeuron* SScNeuron::create(SSeNeuronType type, const QString& name)
{
    SScNeuron* ret = NULL;
    switch(type)
    {
        case NeuronType_Hidden: ret = new (std::nothrow) SScHiddenNeuron(); break;
        case NeuronType_Input:  ret = new (std::nothrow) SScInputNeuron (); break;
        case NeuronType_Output: ret = new (std::nothrow) SScOutputNeuron(); break;
        case NeuronType_Bias:   ret = new (std::nothrow) SScBiasNeuron  (); break;
    }
    ret->setName(name);
    return ret;
}
