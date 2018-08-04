#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP
#include "sscactivation.hpp"
#include "sscconnection.hpp"

class SScNeuron
{
public:
enum SSeNeuronType
{
    NeuronType_Input,
    NeuronType_Hidden,
    NeuronType_Output
};
    SScNeuron(SSeNeuronType type);
    virtual ~SScNeuron();
    virtual bool addInput(SScNeuron* other, double v) = 0;
    virtual bool delInput(SScNeuron* other) = 0;
    virtual double out() = 0;
    virtual double net() = 0;
    virtual double dlt() = 0;
    virtual double dltFwd(SScNeuron* n) = 0;
    virtual bool  setIO(double v) = 0;
    virtual double deltaw(SScNeuron* n) = 0;
    virtual QList<SScNeuron*> inputs() const = 0;
    static SScNeuron* create(SSeNeuronType type);
    inline SSeNeuronType type() const { return m_type; }
private:
    SSeNeuronType m_type;
};
#endif // SSCNEURON_HPP
