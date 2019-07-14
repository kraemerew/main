#ifndef HWN_POOL_HPP
#define HWN_POOL_HPP

#include "neuron.hpp"

class SScPoolNeuron : public SSiHighwayNeuron
{
public:
    SScPoolNeuron(SScHighwayNetwork* net, SSiHighwayNeuron::Type t);
    virtual bool    addInput            (SSiHighwayNeuron* other, SScTrainableParameter*);
    virtual bool    addInput            (SSiHighwayNeuron* other, double, SScTrainableParameter::Type);
    virtual bool    delInput            (SSiHighwayNeuron* other);
    virtual bool    connectHighway      (SSiHighwayNeuron*, SSiHighwayNeuron*) { return false; }
    virtual bool    setActivation       (SScActivation::Type, double) { return false; }
    virtual double  icon                (SSiHighwayNeuron*) { return 0; }
    virtual double  forwardSelectedDedo (SSiHighwayNeuron* ref);

    virtual double  transform   ();
    virtual double  net         ();
    virtual double  out         ()                  { return transform(); }
    virtual void    reset       ()                  {  SSiHighwayNeuron::reset(); m_fwdedoset=false; m_selected=false; m_sel = NULL; }
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
    virtual void priv_poolselect() = 0;
    virtual double priv_dedo() { return 0; }
    bool                        m_selected, m_fwdedoset;
    double                      m_fwdedo;
    SSiHighwayNeuron*           m_sel;                  // The neuron which achieved the maximum
    QList<SSiHighwayNeuron*>    m_in;
};

class SScMaxPoolNeuron : public SScPoolNeuron
{
public:
    SScMaxPoolNeuron(SScHighwayNetwork* net) : SScPoolNeuron(net,MaxPool)
    {}

protected:
    virtual void priv_poolselect();
};

class SScMinPoolNeuron : public SScPoolNeuron
{
public:
    SScMinPoolNeuron(SScHighwayNetwork* net) : SScPoolNeuron(net,MinPool)
    {}

protected:
    virtual void priv_poolselect();
};

class SScMedPoolNeuron : public SScPoolNeuron
{
public:
    SScMedPoolNeuron(SScHighwayNetwork* net) : SScPoolNeuron(net,MedPool)
    {}

protected:
    virtual void priv_poolselect();
};


#endif  // HWN_POOL_HPP
