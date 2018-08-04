#include "sscneuron.hpp"
#include <QtMath>
#include <QMap>

class SScHiddenNeuron : public SScNeuron
{
public:
    SScHiddenNeuron() : SScNeuron(), m_act(SScActivation::create(SScActivation::Act_Tanh))
    {
        Q_CHECK_PTR(m_act);
    }

    bool setIO(double) { Q_ASSERT(false); return false; }
    bool addInput(SScNeuron *other, double v)
    {
        if ((this==other) || m_in.contains(other)) return false;
        m_in[other]=v;
        return true;
    }

    bool delInput(SScNeuron *other)
    {
        if (!m_in.contains(other)) return false;
        m_in.remove(other);
        return true;
    }
   virtual double net()
    {
        double ret = 0;
        for(QMap<SScNeuron*,double>::iterator it = m_in.begin(); it != m_in.end(); ++it) ret += it.key()->out()*it.value();
        return ret;
    }

    virtual double out()
    {
        return m_act->activate(net());
    }

    virtual double dlt()
    {
        double ret = 0;
        foreach(SScNeuron* n, m_out) ret+=n->dltFwd(this);
        ret*=m_act->dev();
        return ret;
    }

    virtual double dltFwd(SScNeuron* n)
    {
        Q_ASSERT(m_in.contains(n));
        return m_in[n]*dlt();
    }

    virtual double deltaw(SScNeuron* n)
    {
        if (m_in.contains(n))
        {
            return -n->out()*dlt(); //< todo multiply local learning rate for connection from n
        }
        return 0.0;
    }
    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }

private:
    QMap<SScNeuron*,double> m_in;
    QList<SScNeuron*>       m_out;
    SScActivation*          m_act;
};



class SScInputNeuron : public SScNeuron
{
public:
    SScInputNeuron() : SScNeuron() {}
    bool addInput(SScNeuron *, double ) { return false; }
    bool delInput(SScNeuron *) { return false; }
    bool setIO(double v) { m_input = v; return true; }

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

private:
    QList<SScNeuron*>       m_out;
    double                  m_input;
};

class SScOutputNeuron : public SScNeuron
{
public:
    SScOutputNeuron() : SScNeuron(), m_act(SScActivation::create(SScActivation::Act_Logistic))
    {
        Q_CHECK_PTR(m_act);
    }
    bool setIO(double v) { m_target = v; return true; }

    bool addInput(SScNeuron *other, double v)
    {
        if ((this==other) || m_in.contains(other)) return false;
        m_in[other]=v;
        return true;
    }

    bool delInput(SScNeuron *other)
    {
        if (!m_in.contains(other)) return false;
        m_in.remove(other);
        return true;
    }

    virtual double net()
    {
        double ret = 0;
        for(QMap<SScNeuron*,double>::iterator it = m_in.begin(); it != m_in.end(); ++it) ret += it.key()->out()*it.value();
        return ret;
    }

    virtual double out()
    {
        return m_act->activate(net());
    }

    virtual double dlt()
    {
        double ret = out()-m_target;
        ret*=m_act->dev();
        return ret;
    }

    virtual double dltFwd(SScNeuron* n)
    {
        Q_ASSERT(m_in.contains(n));
        return m_in[n]*dlt();
    }
    virtual double deltaw(SScNeuron* n)
    {
        if (m_in.contains(n))
        {
            return -n->out()*dlt(); //< todo multiply local learning rate for connection from n
        }
        return 0.0;
    }
    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }

private:
    QMap<SScNeuron*,double> m_in;
    QList<SScNeuron*>       m_out;
    SScActivation*          m_act;
    double                  m_target;
};


SScNeuron* SScNeuron::create(SSeNeuronType type)
{
    switch(type)
    {
    case NeuronType_Hidden: return new (std::nothrow) SScHiddenNeuron();
    case NeuronType_Input:  return new (std::nothrow) SScInputNeuron();
    case NeuronType_Output: return new (std::nothrow) SScOutputNeuron();
    }
    return NULL;
}
