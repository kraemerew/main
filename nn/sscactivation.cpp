#include "sscactivation.hpp"
#include <QtMath>


class SScActivationSigmoid : public SScActivation
{
public:
    SScActivationSigmoid() : SScActivation() {}
    virtual double activate(double net)
    {
        m_net = net;
        m_out = 1.0/(1.0+exp(net));
        return m_out;
    }

    virtual double fn() const { return m_out; }
    virtual double dev() const { return m_out*(1.0-m_out); }
double m_net, m_out;
};
class SScActivationTanh: public SScActivation
{
public:
    SScActivationTanh() : SScActivation(), m_net(0), m_out(0) {}
    virtual double activate(double net)
    {
        m_net = net;
        m_out = tanh(net);
        return m_out;
    }

    virtual double fn() const { return m_out; }
    virtual double dev() const { return (1+m_out)*(1-m_out); }
    double m_net, m_out;

};

SScActivation* SScActivation::create(SSeActivation type)
{
    switch (type)
    {
    case Act_Sigmoid: return new (std::nothrow) SScActivationSigmoid();
    case Act_Tanh:      return new (std::nothrow) SScActivationTanh();
    }
return NULL;
}
