#ifndef HWN_GATED_HPP
#define HWN_GATED_HPP

#include "neuron.hpp"
#include "gate.hpp"

class SScGatedNeuron : public SSiHighwayNeuron
{
public:
    SScGatedNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype);

    virtual bool addConnection(SSiHighwayNeuron* other, SScTrainableParameter* tp);
    virtual bool addConnection(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t);
    virtual bool delConnection(SSiHighwayNeuron *other);

    virtual QList<SSiHighwayNeuron*> inputs() const;
    virtual QList<SSiHighwayNeuron*> allInputs() const;

    virtual bool    setInput    (double) { return false; }
    virtual bool    setTarget   (double) { return false; }

    virtual double  net             ();
    virtual double  icon            (SSiHighwayNeuron*);
    virtual void    reset           ();
    virtual double  carry           ();
    virtual double  highway         ();
    virtual bool    connectHighway  (SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn);
    virtual double  transform       ();
    virtual double  out             ();
    virtual double  deltaw          (SSiHighwayNeuron* n);

    virtual void    trainingStep    ();
    virtual void    endOfCycle      ();

    virtual QVariantMap toVM    () const;
    virtual bool        fromVM  (const QVariantMap &vm);

    void dump();

protected:
    SScGate      m_in;
    SSiHighwayNeuron*   m_hwn;
    SSiHighwayNeuron*   m_cn;
};


class SScHiddenNeuron : public SScGatedNeuron
{
public:
    SScHiddenNeuron(SScHighwayNetwork* net)
        : SScGatedNeuron(net, Hidden, SScActivation::MHAT)
    {}
};


class SScOutputNeuron : public SScGatedNeuron
{
public:

    SScOutputNeuron(SScHighwayNetwork* net)
        : SScGatedNeuron(net,SSiHighwayNeuron::Output,SScActivation::SWISH),
          m_target(0.0)
    {}

    virtual bool    setTarget(double v) { if (m_target==v) return false; m_target = v; return true; }
    virtual double  err() { return out()-m_target; }
    virtual bool addFwdConnection(SSiHighwayNeuron *) { return false; }
    virtual bool delFwdConnection(SSiHighwayNeuron *) { return false; }

private:

    virtual double priv_dedo()
    {
        return 2.0*err();
    }

    double m_target;
};

#endif // HWN_GATED_HPP
