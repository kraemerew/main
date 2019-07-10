#include "ssctrainableparameter.hpp"
#include "sscvm.hpp"
#include <QtMath>

void SScTrainableParameter::update(double v)
{
    m_updatesum += v;
    ++m_ctr;    
}
void SScTrainableParameter::endOfCycle()
{
    if (m_ctr>0)
    {
        const double dlt = m_eta*(m_updatesum/(double)m_ctr);
        m_value+=dlt;
        m_updatesum=0.0;
        m_ctr = 0;
    }
}

QVariantMap SScTrainableParameter::toVM() const
{
    QVariantMap vm;
    vm["TYPE"] = type2Id(m_t);
    vm["VALUE"] = m_value;
    vm["ETA"] = m_eta;
    return vm;
}
bool SScTrainableParameter::fromVM(const QVariantMap &vm)
{
    SScVM sscvm(vm);
    m_eta = sscvm.doubleToken("ETA", m_eta);
    m_value = sscvm.doubleToken("VALUE", m_value);
    return true;
}


class SScTrainableParameterMomentum : public SScTrainableParameter
{
public:
    SScTrainableParameterMomentum(double v, double momentum = .2)
        : SScTrainableParameter (v,MOMENTUM),
          m_momentum            (momentum),
          m_momentumc           (1.0-momentum),
          m_last                (0)
    {
        Q_ASSERT(momentum>=0);
        Q_ASSERT(momentum<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const double now = m_updatesum/(double)m_ctr,
                         dlt = m_eta*(now*m_momentumc + m_last*m_momentum);
            m_value+=dlt;
            m_last=now;
            m_updatesum=0.0;
            m_ctr = 0;
        }
    }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["MOMENTUM"]    = m_momentum;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        SScVM sscvm(vm);
        m_value  = sscvm.doubleToken("VALUE", m_value);
        m_momentum = sscvm.doubleToken("MOMENTUM",    m_momentum);
        m_momentumc = 1.0-m_momentum;
        return true;
    }

private:
    double m_momentum, m_momentumc, m_last;
};

class SScTrainableParameterRProp : public SScTrainableParameter
{
public:
    SScTrainableParameterRProp(double v, double eminus = 0.5, double eplus = 1.2, double uval = .1)
        : SScTrainableParameter (v,RPROP),
          m_eplus               (eplus),
          m_eminus              (eminus),
          m_uval                (uval),
          m_last                (0)
    {
        Q_ASSERT(m_eminus>0);
        Q_ASSERT(m_eminus<1);
        Q_ASSERT(m_eplus>1);
        Q_ASSERT(m_uval>=1e-10);

    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const bool signchange = ((m_updatesum>0) && (m_last<0)) || ((m_updatesum<0) && (m_last>0));
            if (signchange) m_uval*=m_eminus; else m_uval*=m_eplus;
            m_uval=qBound(1e-10,m_uval,1000.0);
            if (m_updatesum>0) m_value+=m_uval; else if (m_updatesum<0) m_value-=m_uval;
            m_last=m_updatesum;
            m_updatesum=0.0;
            m_ctr = 0;
        }
    }
    QVariantMap toVM() const
    {
        QVariantMap vm;
        vm["TYPE"]  = type2Id(m_t);
        vm["VALUE"] = m_value;
        vm["E+"]    = m_eplus;
        vm["E-"]    = m_eminus;
        vm["DLT"]   = m_uval;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        SScVM sscvm(vm);
        m_value  = sscvm.doubleToken("VALUE", m_value);
        m_eminus = sscvm.doubleToken("E-",    m_eminus);
        m_eplus  = sscvm.doubleToken("E+",    m_eplus);
        m_uval   = sscvm.doubleToken("DLT",   m_uval);
        m_uval   = qMax(1e-10,m_uval);
        m_eminus = qBound(1e-10,m_eminus,1-1e-10);
        m_eplus  = qMax(1+1e-10,m_eplus);
        return true;
    }

private:
    double m_eplus, m_eminus, m_uval, m_last;
};

class SScTrainableParameterRMSProp : public SScTrainableParameter
{
public:
    SScTrainableParameterRMSProp(double v, double beta = 0.9)
        : SScTrainableParameter(v,RMSPROP),
          m_beta    (beta),
          m_betac   (1.0-m_beta),
          m_v       (0)
    {
        Q_ASSERT(beta>=0);
        Q_ASSERT(beta<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const double g = m_updatesum/(double)m_ctr;
            m_v = (m_beta*m_v) + (m_betac*g*g);
            const double dlt = m_eta*(1.0/qSqrt(m_v+1e-10))*g;
            m_value+=dlt;
            m_ctr=0;
            m_updatesum = 0;
        }
    }
    virtual bool reset() { m_v=0; return true; }

    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA"] = m_beta;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta = sscvm.doubleToken("BETA", m_beta);
        m_beta = qBound(1e-10,m_beta,1-1e-10);
        m_betac= 1-m_beta;
        return true;
    }

private:
    double  m_beta,
            m_betac,
            m_v;
};


class SScTrainableParameterAdamCorr : public SScTrainableParameter
{
public:
    SScTrainableParameterAdamCorr(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,ADAMCORR),
          m_beta1   (beta1),
          m_beta2   (beta2),
          m_beta1c  (1-m_beta1),
          m_beta2c  (1-m_beta2),
          m_beta1p  (1),
          m_beta2p  (1),
          m_v       (0),
          m_m       (0)
    {
        Q_ASSERT(beta1>=0);
        Q_ASSERT(beta1<1);
        Q_ASSERT(beta2>=0);
        Q_ASSERT(beta2<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            m_beta1p*=m_beta1;
            m_beta2p*=m_beta2;
            const double g = m_updatesum/m_ctr;
            m_m = (m_beta1*m_m) + (m_beta1c*g);
            m_v = (m_beta2*m_v) + (m_beta2c*g*g);

            const double mcorr = m_m/(1-m_beta1p),
                         vcorr = m_v/(1-m_beta2p),
                         dlt   = m_eta*(mcorr/(qSqrt(vcorr)+1e-10));
            m_value+=dlt;
            m_updatesum = 0;
            m_ctr = 0;
        }
    }
    virtual bool reset() { m_m=0; m_v=0; m_beta1p=1; m_beta2p=1; return true; }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA1"] = m_beta1;
        vm["BETA2"] = m_beta2;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta1 = sscvm.doubleToken("BETA1", m_beta1);
        m_beta2 = sscvm.doubleToken("BETA2", m_beta2);
        m_beta1 = qBound(1e-10,m_beta1,1-1e-10);
        m_beta2 = qBound(1e-10,m_beta2,1-1e-10);
        m_beta1c= 1-m_beta1;
        m_beta2c= 1-m_beta2;
        return true;
    }

private:

    double  m_beta1,  m_beta2,
            m_beta1c, m_beta2c,  //< 1-beta
            m_beta1p, m_beta2p,
            m_v, m_m;
};


class SScTrainableParameterAdam : public SScTrainableParameter
{
public:
    SScTrainableParameterAdam(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,ADAM),
          m_beta1   (beta1),
          m_beta2   (beta2),
          m_beta1c  (1-m_beta1),
          m_beta2c  (1-m_beta2),
          m_v       (0),
          m_m       (0)
    {
        Q_ASSERT(beta1>=0);
        Q_ASSERT(beta1<1);
        Q_ASSERT(beta2>=0);
        Q_ASSERT(beta2<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const double g = m_updatesum/m_ctr;
            m_m = (m_beta1*m_m) + (m_beta1c*g);
            m_v = (m_beta2*m_v) + (m_beta2c*g*g);

            const double dlt   = m_eta*(m_m/(qSqrt(m_v)+1e-10));
            m_value+=dlt;
            m_updatesum = 0;
            m_ctr = 0;
        }
    }
    virtual bool reset() { m_m=0; m_v=0; return true; }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA1"] = m_beta1;
        vm["BETA2"] = m_beta2;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta1 = sscvm.doubleToken("BETA1", m_beta1);
        m_beta2 = sscvm.doubleToken("BETA2", m_beta2);
        m_beta1 = qBound(1e-10,m_beta1,1-1e-10);
        m_beta2 = qBound(1e-10,m_beta2,1-1e-10);
        m_beta1c= 1-m_beta1;
        m_beta2c= 1-m_beta2;
        return true;
    }

private:

    double  m_beta1,  m_beta2,
            m_beta1c, m_beta2c,  //< 1-beta
            m_v, m_m;
};


class SScTrainableParameterAdamax : public SScTrainableParameter
{
public:
    SScTrainableParameterAdamax(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,ADAMAX),
          m_beta1   (beta1),
          m_beta2   (beta2),
          m_beta1c  (1-m_beta1),
          m_u       (0),
          m_m       (0)
    {
        Q_ASSERT(beta1>=0);
        Q_ASSERT(beta1<1);
        Q_ASSERT(beta2>=0);
        Q_ASSERT(beta2<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const double g = m_updatesum/m_ctr;
            m_m = (m_beta1*m_m) + (m_beta1c*g);
            m_u = qMax(m_beta2*m_u,qAbs(g));

            const double dlt   = m_eta*(m_m/(m_u+1e-10));
            m_value+=dlt;
            m_updatesum = 0;
            m_ctr = 0;
        }
    }
    virtual bool reset() { m_m=0; m_u=0; return true; }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA1"] = m_beta1;
        vm["BETA2"] = m_beta2;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta1 = sscvm.doubleToken("BETA1", m_beta1);
        m_beta2 = sscvm.doubleToken("BETA2", m_beta2);
        m_beta1 = qBound(1e-10,m_beta1,1-1e-10);
        m_beta2 = qBound(1e-10,m_beta2,1-1e-10);
        m_beta1c= 1-m_beta1;
        return true;
    }

private:

    double  m_beta1,  m_beta2,
            m_beta1c,  //< 1-beta
            m_u, m_m;
};


class SScTrainableParameterAdamaxCorr : public SScTrainableParameter
{
public:
    SScTrainableParameterAdamaxCorr(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,ADAMAXCORR),
          m_beta1   (beta1),
          m_beta2   (beta2),
          m_beta1c  (1-m_beta1),
          m_u       (0),
          m_m       (0)
    {
        Q_ASSERT(beta1>=0);
        Q_ASSERT(beta1<1);
        Q_ASSERT(beta2>=0);
        Q_ASSERT(beta2<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            m_beta1p*=m_beta1;
            const double g = m_updatesum/m_ctr;
            m_m = (m_beta1*m_m) + (m_beta1c*g);
            m_u = qMax(m_beta2*m_u,qAbs(g));
            const double mcorr = m_m/(1-m_beta1p),
                         dlt   = m_eta*(mcorr/(m_u+1e-10));
            m_value+=dlt;
            m_updatesum = 0;
            m_ctr = 0;
        }
    }
    virtual bool reset() { m_m=0; m_u=0; m_beta1p=1; return true; }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA1"] = m_beta1;
        vm["BETA2"] = m_beta2;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta1 = sscvm.doubleToken("BETA1", m_beta1);
        m_beta2 = sscvm.doubleToken("BETA2", m_beta2);
        m_beta1 = qBound(1e-10,m_beta1,1-1e-10);
        m_beta2 = qBound(1e-10,m_beta2,1-1e-10);
        m_beta1c= 1-m_beta1;
        return true;
    }

private:

    double  m_beta1,  m_beta2,
            m_beta1c,  //< 1-beta
            m_beta1p,
            m_u, m_m;
};


class SScTrainableParameterAMSGrad : public SScTrainableParameter
{
public:
    SScTrainableParameterAMSGrad(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,ADAM),
          m_beta1   (beta1),
          m_beta2   (beta2),
          m_beta1c  (1-m_beta1),
          m_beta2c  (1-m_beta2),
          m_v       (0),
          m_m       (0)
    {
        Q_ASSERT(beta1>=0);
        Q_ASSERT(beta1<1);
        Q_ASSERT(beta2>=0);
        Q_ASSERT(beta2<1);
    }
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const double g = m_updatesum/m_ctr,
                         v = (m_beta2*m_v) + (m_beta2c*g*g),
                      vmax = qMax(m_v,v);

            m_m = (m_beta1*m_m) + (m_beta1c*g);
            m_v = v;

            const double dlt   = m_eta*(m_m/(qSqrt(vmax)+1e-10));
            m_value+=dlt;
            m_updatesum = 0;
            m_ctr = 0;
        }
    }
    virtual bool reset() { m_m=0; m_v=0; return true; }
    QVariantMap toVM() const
    {
        QVariantMap vm = SScTrainableParameter::toVM();
        vm["BETA1"] = m_beta1;
        vm["BETA2"] = m_beta2;
        return vm;
    }
    bool fromVM(const QVariantMap &vm)
    {
        (void) SScTrainableParameter::fromVM(vm);
        SScVM sscvm(vm);
        m_beta1 = sscvm.doubleToken("BETA1", m_beta1);
        m_beta2 = sscvm.doubleToken("BETA2", m_beta2);
        m_beta1 = qBound(1e-10,m_beta1,1-1e-10);
        m_beta2 = qBound(1e-10,m_beta2,1-1e-10);
        m_beta1c= 1-m_beta1;
        m_beta2c= 1-m_beta2;
        return true;
    }

private:

    double  m_beta1,  m_beta2,
            m_beta1c, m_beta2c,  //< 1-beta
            m_v, m_m;
};

QString SScTrainableParameter::name(Type t)
{
    switch(t)
    {
        case STD:       return "Std";       break;
        case MOMENTUM:  return "Momentum";  break;
        case RPROP:     return "RProp";     break;
        case RMSPROP:   return "RmsProp";   break;
        case AMSGRAD:   return "AMSGrad";   break;
        case ADAM:      return "Adam";      break;
        case ADAMCORR:  return "AdamCorr";  break;
        case ADAMAX:    return "AdaMax";    break;
        case ADAMAXCORR:return "AdaMaxCorr";break;
        default:                            break;
    }
    return "";
}

SScTrainableParameter* SScTrainableParameter::create(const QVariantMap& vm)
{
    SScVM sscvm(vm);
    bool ok = false;
    const Type t = id2Type(sscvm.stringToken("TYPE",""),ok);
    if (!ok) return NULL;
    const double v = sscvm.doubleToken("VALUE",0.0);
    SScTrainableParameter* ret = create(t,v);
    if (ret && ret->fromVM(vm)) return ret;
    if (ret) delete ret;
    return NULL;
}

SScTrainableParameter* SScTrainableParameter::create(Type type, double v)
{
    switch(type)
    {
    case STD:       return new SScTrainableParameter            (v); break;
    case MOMENTUM:  return new SScTrainableParameterMomentum    (v); break;
    case RPROP:     return new SScTrainableParameterRProp       (v); break;
    case RMSPROP:   return new SScTrainableParameterRMSProp     (v); break;
    case AMSGRAD:   return new SScTrainableParameterAMSGrad     (v); break;
    case ADAMCORR:  return new SScTrainableParameterAdamCorr    (v); break;
    case ADAM:      return new SScTrainableParameterAdam        (v); break;
    case ADAMAXCORR:return new SScTrainableParameterAdamaxCorr  (v); break;
    case ADAMAX:    return new SScTrainableParameterAdamax      (v); break;

    default:break;
    }
    return NULL;
}

QString SScTrainableParameter::type2Id(Type type)
{
  return name(type).toUpper();
}

SScTrainableParameter::Type SScTrainableParameter::id2Type(const QString &id, bool &ok)
{
    ok = true;
    for (int i = (int)STD; i<(int)LAST; ++i)
        if (type2Id((Type)i)==id.toUpper()) return (Type)i;
    ok = false;
    return LAST;
}
