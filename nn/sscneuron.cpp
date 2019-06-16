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
            m_uval=qBound(0.00001,m_uval,1.0);
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
    virtual void priv_activate() { m_act = m_pot; }
    virtual double priv_dev() { return 1.0; }
};
class SScActivationSigmoid : public SScActivation
{
public:
    SScActivationSigmoid() : SScActivation() {}
    virtual QString name() const { return "Logistic"; }
private:
    virtual void priv_activate() { m_act = 1.0/(1.0+exp(-m_pot)); }
    virtual double priv_dev() { return m_act*(1.0-m_act); }
};

class SScActivationTanh : public SScActivation
{
public:
    SScActivationTanh(): SScActivation() {}
    virtual QString name() const { return "Tanh"; }

private:
    virtual void priv_activate() { m_act = tanh(m_pot); }
    virtual double priv_dev() { return 1.0-qPow(m_act,2.0); }
};

SScActivation* SScActivation::create(SSeActivation type)
{
    switch (type)
    {
    case ACT_IDENTITY:  return new SScActivationIdentity(); break;
    case ACT_SIGMOID:   return new SScActivationSigmoid(); break;
    case ACT_TANH:      return new SScActivationTanh(); break;
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
    virtual double deltaw(SScNeuron* n)
    {
        return -n->out()*dlt();
    }
    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }
    virtual double dltFwd(SScNeuron* n)
    {
        return m_in.contains(n) ? m_in[n]->value()*dlt() : 0.0;
    }
    virtual bool trainingStep(bool cycleDone)
    {
        for(QMap<SScNeuron*,QSharedPointer<SScConnection> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        {
            const double grad = deltaw(it.key());
            it.value()->update(grad,cycleDone);
        }
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

    virtual double dlt()
    {
        double ret = 0;
        foreach(SScNeuron* n, m_out) ret+=n->dltFwd(this);
        ret*=m_act->dev();
        return ret;
    }

    virtual void connectForward(const QList<SScNeuron*>& fwd)
    {
        qWarning("Hidden neuron %s forwards to:", qPrintable(name()));
        foreach (SScNeuron* to, fwd) qWarning("    -> %s", qPrintable(to->name()));
        m_out = fwd;
    }


private:
    QList<SScNeuron*>       m_out;
};



class SScInputNeuron : public SScNeuron
{
public:
    SScInputNeuron() : SScNeuron(NeuronType_Input) {}
    bool addInput(SScNeuron *, double ) { return false; }
    bool delInput(SScNeuron *) { return false; }

    virtual bool  setInput(double v) { m_input = v; return true; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    virtual double deltaw(SScNeuron*) { return 0; }
    virtual double net() { return m_input; }
    virtual double out() { return net(); }
    virtual double dlt()
    {
        double ret = 0;
        foreach(SScNeuron* n, m_out) ret+=n->dltFwd(this);
        return ret;
    }
    virtual double dltFwd(SScNeuron*) { return 0; }
    virtual QList<SScNeuron*> inputs() const { return QList<SScNeuron*>();  }
    virtual bool trainingStep(bool) { return false; }
    virtual void connectForward(const QList<SScNeuron*>& fwd)
    {
        qWarning("Input neuron %s forwards to:", qPrintable(name()));
        foreach (SScNeuron* to, fwd) qWarning("    -> %s", qPrintable(to->name()));
        m_out = fwd;
    }

private:
    QList<SScNeuron*>       m_out;
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
    virtual double  deltaw      (SScNeuron*)            { return 0.0; }
    virtual double  net         ()                      { return 1.0; }
    virtual double  out         ()                      { return 1.0; }
    virtual double  dlt         ()
    {
        double ret = 0;
        foreach(SScNeuron* n, m_out) ret+=n->dltFwd(this);
        return ret;
    }
    virtual double dltFwd(SScNeuron*) { return 0; }
    virtual QList<SScNeuron*> inputs() const { return QList<SScNeuron*>();  }
    virtual bool trainingStep(bool) { return false; }
    virtual void connectForward(const QList<SScNeuron*>& fwd)
    {
        qWarning("Bias neuron %s forwards to:", qPrintable(name()));
        foreach (SScNeuron* to, fwd) qWarning("    -> %s", qPrintable(to->name()));
        m_out = fwd;
    }

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
    virtual double  dlt() { return err()*m_act->dev(); }
    virtual void    connectForward(const QList<SScNeuron*>&) { }

private:
    double                  m_target;
};

SScNeuron::SScNeuron(SSeNeuronType type) : m_type(type), m_act(NULL)
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
