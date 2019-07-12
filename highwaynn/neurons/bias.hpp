#ifndef HWN_BIAS_HPP
#define HWN_BIAS_HPP

#include "neuron.hpp"

class SScBiasNeuron : public SSiHighwayNeuron
{
public:
    SScBiasNeuron(SScHighwayNetwork* net) : SSiHighwayNeuron(net,Bias,SScActivation::IDENTITY)
    {}
    virtual bool    addInput        (SSiHighwayNeuron*, double,SScTrainableParameter::Type) { return false; }
    virtual bool    addInput        (SSiHighwayNeuron*, SScTrainableParameter*)             { return false; }
    virtual bool    delInput        (SSiHighwayNeuron*)                                     { return false; }
    virtual bool    setInput        (double)                                                { return false; }
    virtual bool    setTarget       (double)                                                { return false; }
    virtual double  deltag          ()                                                      { return 0.0; }
    virtual double  deltaw          (SSiHighwayNeuron*)                                     { return 0.0; }
    virtual double  net             ()                                                      { return 1.0; }
    virtual double  out             ()                                                      { return 1.0; }
    virtual bool    connectHighway  (SSiHighwayNeuron*, SSiHighwayNeuron*)                  { return false; }
};

#endif // HWN_BIAS_HPP
