#include "sscneuron.hpp"
#include "QSharedPointer"
#include <QtMath>
#include <QMap>

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
        m_in[other]=QSharedPointer<SScConnection>(new SScConnection(SScConnection::Connectiontype_RPROP,v));
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
        return m_in.contains(n) ? -n->out()*dlt() : 0;
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
            //qWarning("GRADIENT %lf", grad);
            it.value()->update(grad,cycleDone);
        }
        return true;
    }

    QMap<SScNeuron*,QSharedPointer<SScConnection> > m_in;
};

class SScHiddenNeuron : public SScConnectedNeuron
{
public:
    SScHiddenNeuron() : SScConnectedNeuron(NeuronType_Hidden), m_act(SScActivation::create(SScActivation::Act_Tanh))
    {
        Q_CHECK_PTR(m_act);
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

    virtual void connectForward(const QList<SScNeuron*>& fwd) { m_out = fwd; }


private:
    QList<SScNeuron*>       m_out;
    SScActivation*          m_act;
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
    virtual void connectForward(const QList<SScNeuron*>& fwd) { m_out = fwd; }

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
    virtual void connectForward(const QList<SScNeuron*>& fwd) { m_out = fwd; }

private:
    QList<SScNeuron*>       m_out;
};

class SScOutputNeuron : public SScConnectedNeuron
{
public:
    SScOutputNeuron() : SScConnectedNeuron(NeuronType_Output), m_act(SScActivation::create(SScActivation::Act_Logistic))
    {
        Q_CHECK_PTR(m_act);
    }

    virtual bool    setInput(double) { Q_ASSERT(false); return false; }
    virtual bool    setTarget(double v) { m_target = v; return true; }
    virtual double  err () { return out()-m_target; }
    virtual double  out() { return m_act->activate(net()); }
    virtual double  dlt() { return err()*m_act->dev(); }
    virtual void    connectForward(const QList<SScNeuron*>&) { }

private:
    SScActivation*          m_act;
    double                  m_target;
};

SScNeuron::SScNeuron(SSeNeuronType type) : m_type(type) {}
SScNeuron::~SScNeuron() {}

SScNeuron* SScNeuron::create(SSeNeuronType type)
{
    switch(type)
    {
    case NeuronType_Hidden: return new (std::nothrow) SScHiddenNeuron();
    case NeuronType_Input:  return new (std::nothrow) SScInputNeuron ();
    case NeuronType_Output: return new (std::nothrow) SScOutputNeuron();
    case NeuronType_Bias:   return new (std::nothrow) SScBiasNeuron  ();
    }
    return NULL;
}
