#ifndef HWN_POOL_HPP
#define HWN_POOL_HPP

#include "highwayneuron.hpp"

class SScPoolNeuron : public SSiHighwayNeuron
{
public:
    SScPoolNeuron(SScHighwayNetwork* net);
    virtual bool    addInput            (SSiHighwayNeuron* other, SScTrainableParameter*);
    virtual bool    addInput            (SSiHighwayNeuron* other, double, SScTrainableParameter::Type);
    virtual bool    delInput            (SSiHighwayNeuron* other);
    virtual bool    connectHighway      (SSiHighwayNeuron*, SSiHighwayNeuron*) { return false; }
    virtual bool    setActivation       (SScActivation::Type, double) { return false; }
    virtual double  icon                (SSiHighwayNeuron*) { return 0.0; }
    virtual double  forwardSelectedDedo (SSiHighwayNeuron* ref);

    virtual double  transform   ();
    virtual double  net         ();
    virtual double  out         ()                  { return transform(); }
    virtual void    reset       ()                  { SSiHighwayNeuron::reset(); m_selected=false; m_sel = NULL; }
    virtual bool    setInput    (double)            { return false; }
    virtual bool    setTarget   (double)            { return false; }
    virtual double  deltag      ()                  { return 0; }
    virtual double  deltaw      (SSiHighwayNeuron*) { return 0; }
    virtual void    trainingStep()                  {}
    virtual void    endOfCycle  ()                  {}

    virtual QVariantMap toVM() const;
    virtual bool        fromVM(const QVariantMap &vm);

    virtual QList<SSiHighwayNeuron*> inputs     () const { return m_in; }
    virtual QList<SSiHighwayNeuron*> allInputs  () const { return inputs(); }

    void dump();

protected:
    virtual double priv_dedo() { return 0; }
    bool                        m_selected;
    SSiHighwayNeuron*           m_sel;                  // The neuron which achieved the maximum
    QList<SSiHighwayNeuron*>    m_in;
};

#endif  // HWN_POOL_HPP
