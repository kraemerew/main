#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP
#include "sscactivation.hpp"

class SScNeuron
{
enum SSeNeuronType
{
    NeuronType_Input,
    NeuronType_Hidden,
    NeuronType_Output
};
public:
    SScNeuron() {}
    virtual bool addInput(SScNeuron* other, double v) = 0;
    virtual bool delInput(SScNeuron* other) = 0;
    virtual double out() = 0;
    virtual double net() = 0;
    virtual double dlt() = 0;
    virtual double dltFwd(SScNeuron* n) = 0;
    virtual bool  setIO(double v) = 0;
    virtual double deltaw(SScNeuron* n) = 0;
    static SScNeuron* create(SSeNeuronType type);
};
#endif // SSCNEURON_HPP
