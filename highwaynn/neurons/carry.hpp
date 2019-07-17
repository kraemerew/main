#ifndef HWN_CARRY_HPP
#define HWN_CARRY_HPP

#include "neuron.hpp"
#include "gate.hpp"

class SScCarryNeuron : public SSiHighwayNeuron
{
public:
    SScCarryNeuron(SScHighwayNetwork* net);
    virtual double transform();

    virtual double out() { return transform(); }
    virtual bool   addInput(SSiHighwayNeuron* other, SScTrainableParameter* tp);
    virtual bool   addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t);
    virtual bool   delInput(SSiHighwayNeuron *other);
    virtual double net();
    virtual void   reset();
    virtual bool   setInput(double) { Q_ASSERT(false); return false; }
    virtual bool   setTarget(double) { Q_ASSERT(false); return false; }
    virtual bool   connectHighway(SSiHighwayNeuron*, SSiHighwayNeuron*) { Q_ASSERT(false); return false; }
    virtual double deltaw(SSiHighwayNeuron* n);
    virtual bool   setActivation(SScActivation::Type type, double gain);
    virtual void   trainingStep();
    virtual void   endOfCycle();
    virtual bool   fromVM(const QVariantMap&);
    virtual QVariantMap toVM() const;
    void dump();

    virtual QList<SSiHighwayNeuron*> inputs() const { return m_in.keys(); }
    virtual QList<SSiHighwayNeuron*> allInputs() const { return inputs(); }
    virtual double icon(SSiHighwayNeuron *other) { return m_in.contains(other) ? m_in[other]->value() : 0.0; }

protected:
    virtual double priv_dedo();
    SScGate m_in;
};

#endif // HWN_CARRY_HPP
