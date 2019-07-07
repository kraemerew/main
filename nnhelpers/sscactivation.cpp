#include "sscactivation.hpp"
#include <QtMath>

class SScActivationIdentity : public SScActivation
{
public:
    SScActivationIdentity(): SScActivation(ACT_IDENTITY) {}

private:
    virtual void priv_activate() { m_act = m_pot*m_gain->value(); }
    virtual double priv_dev() { return m_gain->value(); }
};
class SScActivationSigmoid : public SScActivation
{
public:
    SScActivationSigmoid() : SScActivation(ACT_SIGMOID) {}

private:
    virtual void priv_activate() { m_act = 1.0/(1.0+exp(-m_pot*m_gain->value())); }
    virtual double priv_dev() { return m_act*(1.0-m_act); }    
};

class SScActivationTanh : public SScActivation
{
public:
    SScActivationTanh(): SScActivation(ACT_TANH) {}

private:
    virtual void priv_activate() { m_act = tanh(m_pot*m_gain->value()); }
    virtual double priv_dev() { return 1.0-qPow(m_act,2.0); }
};

class SScActivationRbf: public SScActivation
{
public:
    SScActivationRbf() : SScActivation(ACT_RBF) {}

private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_act = exp(-qPow(m_gp,2.0)); }
    virtual double priv_dev() { return -2.0*m_gp*m_act; }
    double m_gp;
};


class SScActivationMHat: public SScActivation
{
public:
    SScActivationMHat() : SScActivation(ACT_MHAT) {}

private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_gp2 = qPow(m_gp,2.0); m_act = (1.0-m_gp2)*exp(-m_gp2); }
    virtual double priv_dev() { return (m_gp==1.0) ? -2.0/exp(1) : -2.0*m_gp*m_act*(1+1.0/(1.0-m_gp2)); }
    double m_gp, m_gp2;
};

class SScActivationGaussianDerivative : public SScActivation
{
public:
    SScActivationGaussianDerivative() : SScActivation(ACT_GDER) {}

private:
    virtual void priv_activate() { m_x = m_pot*m_gain->value(); m_act = -2.0*m_x*exp(-qPow(m_x,2.0)); }
    virtual double priv_dev() { return (m_x!=0.0) ? ((1.0/m_x)-(2.0*m_x)) * m_act : -2.0; }
    double m_x;
};

class SScActivationSoftPlus : public SScActivation
{
public:
    SScActivationSoftPlus() : SScActivation(ACT_SOFTPLUS) {}

private:
    virtual void priv_activate() { m_e = exp(m_pot*m_gain->value()); m_act = log(1+m_e); }
    virtual double priv_dev() { return m_e/(1.0+m_e); }
    double m_e;
};

class SScActivationX : public SScActivation
{
public:
    SScActivationX() : SScActivation(ACT_X) {}

private:
    virtual void priv_activate() { m_x = m_pot*m_gain->value(); m_xp=qPow(m_x,2.0);  m_act = 2.0*(m_x/(m_xp+1.0)); }
    virtual double priv_dev() { return 2.0*(1.0-m_xp)/(qPow(m_xp+1,2)); }
    double m_x, m_xp;
};

class SScActivationSwish : public SScActivation
{
public:
    SScActivationSwish() : SScActivation(ACT_SWISH) {}

private:
    virtual void priv_activate() { const double m_x = m_pot*m_gain->value();  m_actsig = 1.0/(1.0+exp(-m_x)); m_act = m_x*m_actsig; }
    virtual double priv_dev() { return m_actsig+m_x*m_actsig*(1-m_actsig); }
    double m_x, m_actsig;
};

bool SScActivation::nonLinear(Type type) { return type!=ACT_IDENTITY; }
bool SScActivation::canCarry(Type type)
{
    switch (type)
    {
    case ACT_SIGMOID:
    case ACT_RBF:       return true; break;
    default: return false; break;
    }
}
SScActivation* SScActivation::create(Type type)
{
    switch (type)
    {
    case ACT_IDENTITY:  return new SScActivationIdentity(); break;
    case ACT_SIGMOID:   return new SScActivationSigmoid(); break;
    case ACT_TANH:      return new SScActivationTanh(); break;
    case ACT_RBF:       return new SScActivationRbf(); break;
    case ACT_SOFTPLUS:  return new SScActivationSoftPlus(); break;
    case ACT_SWISH:     return new SScActivationSwish(); break;
    case ACT_MHAT:      return new SScActivationMHat(); break;
    case ACT_GDER:      return new SScActivationGaussianDerivative(); break;
    case ACT_X:         return new SScActivationX(); break;
    }
    return NULL;
}

QString SScActivation::name(Type type)
{
    switch (type)
    {
    case ACT_IDENTITY:  return "Id"; break;
    case ACT_SIGMOID:   return "Logistic"; break;
    case ACT_TANH:      return "Tanh"; break;
    case ACT_RBF:       return "Rbf"; break;
    case ACT_SOFTPLUS:  return "SoftPlus"; break;
    case ACT_SWISH:     return "Swish"; break;
    case ACT_MHAT:      return "MexHat"; break;
    case ACT_GDER:      return "GDer"; break;
    case ACT_X:         return "X"; break;
    }
    return "";
}

void SScActivation::setTrainingType(SScTrainableParameter::Type t)
{
    if (m_gain->trainingType()!=t)
    {
        const double v = m_gain->value();
        delete m_gain;
        m_gain = SScTrainableParameter::create(t,v);
        Q_CHECK_PTR(m_gain);
    }
}
