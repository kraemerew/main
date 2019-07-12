#ifndef HWN_INPUT_HPP
#define HWN_INPUT_HPP
#include "neuron.hpp"

class SScInputNeuron : public SSiHighwayNeuron
{
public:
    SScInputNeuron(SScHighwayNetwork* net)
        : SSiHighwayNeuron(net,Input, SScActivation::IDENTITY),
          m_input(0.0)
    {}

    virtual bool    addInput        (SSiHighwayNeuron*, double,SScTrainableParameter::Type) { return false; }
    virtual bool    addInput        (SSiHighwayNeuron*, SScTrainableParameter*)             { return false; }
    virtual bool    delInput        (SSiHighwayNeuron*)                                     { return false; }
    virtual bool    setInput        (double v)                                              { if (v==m_input) return false; m_input = v; return true; }
    virtual bool    setTarget       (double)                                                { return false; }
    virtual double  deltag          ()                                                      { return 0.0; }
    virtual double  deltaw          (SSiHighwayNeuron*)                                     { return 0; }
    virtual double  net             ()                                                      { return m_input; }
    virtual double  out             ()                                                      { return m_input; }
    virtual bool    connectHighway  (SSiHighwayNeuron*, SSiHighwayNeuron*)                  { return false; }

private:
    double  m_input;
};

#endif // HWN_INPUT_HPP
