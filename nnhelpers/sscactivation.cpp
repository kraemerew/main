#include "sscactivation.hpp"
#include "../nnhelpers/sscvm.hpp"
#include <QtMath>

class SScActivationIdentity : public SScActivation
{
public:
    SScActivationIdentity(): SScActivation(IDENTITY) {}

private:
    virtual void priv_activate() { m_act = m_pot*m_gain->value(); }
    virtual double priv_dev() { return m_gain->value(); }
};
class SScActivationSigmoid : public SScActivation
{
public:
    SScActivationSigmoid() : SScActivation(LOGISTIC) {}

private:
    virtual void priv_activate() { m_act = 1.0/(1.0+exp(-m_pot*m_gain->value())); }
    virtual double priv_dev() { return m_act*(1.0-m_act); }    
};


class SScActivationRelu : public SScActivation
{
public:
    SScActivationRelu(): SScActivation(RELU) {}

private:
    virtual void priv_activate() { m_act = (m_pot<0) ? 0.0 : m_gain->value()*m_pot; }
    virtual double priv_dev() { return (m_pot<0) ? 0 : m_gain->value(); }
};

class SScActivationTanh : public SScActivation
{
public:
    SScActivationTanh(): SScActivation(TANH) {}

private:
    virtual void priv_activate() { m_act = tanh(m_pot*m_gain->value()); }
    virtual double priv_dev() { return 1.0-qPow(m_act,2.0); }
};

class SScActivationRbf: public SScActivation
{
public:
    SScActivationRbf() : SScActivation(RBF) {}

private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_act = exp(-qPow(m_gp,2.0)); }
    virtual double priv_dev() { return -2.0*m_gp*m_act; }
    double m_gp;
};


class SScActivationMHat: public SScActivation
{
public:
    SScActivationMHat() : SScActivation(MHAT) {}

private:
    virtual void priv_activate() { m_gp = m_pot*m_gain->value(); m_gp2 = qPow(m_gp,2.0); m_act = (1.0-m_gp2)*exp(-m_gp2); }
    virtual double priv_dev() { return (m_gp==1.0) ? -2.0/exp(1) : -2.0*m_gp*m_act*(1+1.0/(1.0-m_gp2)); }
    double m_gp, m_gp2;
};

class SScActivationGaussianDerivative : public SScActivation
{
public:
    SScActivationGaussianDerivative() : SScActivation(GDER) {}

private:
    virtual void priv_activate() { m_x = m_pot*m_gain->value(); m_act = -2.0*m_x*exp(-qPow(m_x,2.0)); }
    virtual double priv_dev() { return (m_x!=0.0) ? ((1.0/m_x)-(2.0*m_x)) * m_act : -2.0; }
    double m_x;
};

class SScActivationSoftPlus : public SScActivation
{
public:
    SScActivationSoftPlus() : SScActivation(SOFTPLUS) {}

private:
    virtual void priv_activate() { m_e = exp(m_pot*m_gain->value()); m_act = log(1+m_e); }
    virtual double priv_dev() { return m_e/(1.0+m_e); }
    double m_e;
};

class SScActivationX : public SScActivation
{
public:
    SScActivationX() : SScActivation(X) {}

private:
    virtual void priv_activate() { m_x = m_pot*m_gain->value(); m_xp=qPow(m_x,2.0);  m_act = 2.0*(m_x/(m_xp+1.0)); }
    virtual double priv_dev() { return 2.0*(1.0-m_xp)/(qPow(m_xp+1,2)); }
    double m_x, m_xp;
};

class SScActivationSwish : public SScActivation
{
public:
    SScActivationSwish() : SScActivation(SWISH) {}

private:
    virtual void priv_activate() { const double m_x = m_pot*m_gain->value();  m_actsig = 1.0/(1.0+exp(-m_x)); m_act = m_x*m_actsig; }
    virtual double priv_dev() { return m_actsig+m_x*m_actsig*(1-m_actsig); }
    double m_x, m_actsig;
};

bool SScActivation::nonLinear(Type type) { return type!=IDENTITY; }
bool SScActivation::canCarry(Type type)
{
    switch (type)
    {
    case LOGISTIC:
    case RBF:       return true; break;
    default: return false; break;
    }
}
SScActivation* SScActivation::create(const QVariantMap& vm)
{
    SScVM sscvm(vm);
    bool ok = false;
    auto t = id2Type(sscvm.stringToken("TYPE",""),ok);
    if (!ok || (t==LAST)) t = IDENTITY;
    SScActivation* ret = create(t);
    if (ret) ret->fromVM(vm);
    return ret;
}
SScActivation* SScActivation::create(Type type)
{
    switch (type)
    {
    case IDENTITY:  return new SScActivationIdentity();             break;
    case RELU:      return new SScActivationRelu();                 break;
    case LOGISTIC:   return new SScActivationSigmoid();             break;
    case TANH:      return new SScActivationTanh();                 break;
    case RBF:       return new SScActivationRbf();                  break;
    case SOFTPLUS:  return new SScActivationSoftPlus();             break;
    case SWISH:     return new SScActivationSwish();                break;
    case MHAT:      return new SScActivationMHat();                 break;
    case GDER:      return new SScActivationGaussianDerivative();   break;
    case X:         return new SScActivationX();                    break;
    default: break;
    }
    return NULL;
}

QString SScActivation::name(Type type)
{
    switch (type)
    {
    case IDENTITY:  return "Id"; break;
    case RELU:      return "Relu"; break;
    case LOGISTIC:  return "Logistic"; break;
    case TANH:      return "Tanh"; break;
    case RBF:       return "Rbf"; break;
    case SOFTPLUS:  return "SoftPlus"; break;
    case SWISH:     return "Swish"; break;
    case MHAT:      return "MexHat"; break;
    case GDER:      return "GDer"; break;
    case X:         return "X"; break;
    default: break;
    }
    return "";
}

QString SScActivation::type2Id(Type type)
{
    return name(type).toUpper();
}
SScActivation::Type SScActivation::id2Type(const QString &id, bool &ok)
{
   ok = true;
   for (int i = (int) IDENTITY; i<(int)LAST; ++i)
       if (id.toUpper()==type2Id((Type)i)) return (Type)i;
   ok = false;
   return LAST;
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

QVariantMap SScActivation::toVM() const
{
    QVariantMap vm;
    vm["TYPE"] = type2Id(m_t);
    vm["GAIN"] = m_gain->toVM();
    return vm;
}

bool SScActivation::fromVM(const QVariantMap& vm)
{
    Q_UNUSED(vm);
    //TODO
    return false;
}
