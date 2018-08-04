#include "sscactivation.hpp"
#include <QtMath>

class SScActivationIdentity : public SScActivation
{
public:
    SScActivationIdentity() : SScActivation() {}
    virtual double activate(double net)
    {
        m_net = net;
        return m_net;
    }

    virtual double fn() const { return m_net; }
    virtual double dev() const { return 1.0; };
    double m_net;
};
class SScActivationLogistic : public SScActivation
{
public:
    SScActivationLogistic() : SScActivation(), m_net(0), m_out(0) {}
    virtual double activate(double net)
    {
        m_net = net;
        m_out = 1.0/(1.0+exp(-net));
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
    virtual double dev() const { return 1.0-(m_out*m_out); }
    double m_net, m_out;
};

class SScActivationRbf: public SScActivation
{
public:
    SScActivationRbf() : SScActivation(), m_net(0), m_out(0) {}
    virtual double activate(double net)
    {
        m_net = net;
        m_out = exp(-net*net);
        return m_out;
    }

    virtual double fn() const { return m_out; }
    virtual double dev() const { return -2.0*m_net*m_out; }
    double m_net, m_out;
};

class SScActivationMHat: public SScActivation
{
public:
    SScActivationMHat() : SScActivation(), m_net(0), m_netsq(0), m_out(0) {}
    virtual double activate(double net)
    {
        m_net = net;
        m_netsq = net*net;
        m_out = (1-m_netsq)*exp(-m_netsq);
        return m_out;
    }

    virtual double fn() const { return m_out; }
    virtual double dev() const
    {
        return (m_netsq==1.0) ? (-2.0/M_E) : (-2.0*m_net*m_out*(1.0+(1.0/(1.0-m_netsq))));
    }
    double m_net, m_netsq, m_out;
};
class SScActivationSoftmax: public SScActivation
{
public:
    SScActivationSoftmax() : SScActivation(), m_net(0), m_out(0), m_exp(0) {}
    virtual double activate(double net)
    {
        m_net = net;
        m_exp = exp(net);
        m_out = qLn(1+m_exp);
        return m_out;
    }

    virtual double fn() const { return m_out; }
    virtual double dev() const { return (m_exp)/(1+m_exp); }
    double m_net, m_out, m_exp;
};

SScActivation* SScActivation::create(SSeActivation type)
{
    switch (type)
    {
    case Act_Identity:  return new (std::nothrow) SScActivationIdentity ();
    case Act_Logistic:  return new (std::nothrow) SScActivationLogistic ();
    case Act_Tanh:      return new (std::nothrow) SScActivationTanh     ();
    case Act_Rbf:       return new (std::nothrow) SScActivationRbf      ();
    case Act_MHat:      return new (std::nothrow) SScActivationMHat     ();
    case Act_Softmax:   return new (std::nothrow) SScActivationSoftmax  ();
    }
return NULL;
}
