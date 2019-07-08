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
    vm["TYPE"] = m_t;
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

class SScConnectionRProp : public SScTrainableParameter
{
public:
    SScConnectionRProp(double v, double eminus = 0.5, double eplus = 1.2, double uval = .1)
        : SScTrainableParameter (v,CON_RPROP),
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
        vm["TYPE"]  = m_t;
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

class SScConnectionRMSProp : public SScTrainableParameter
{
public:
    SScConnectionRMSProp(double v, double beta = 0.9)
        : SScTrainableParameter(v,CON_RMSPROP),
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


class SScConnectionAdamCorr : public SScTrainableParameter
{
public:
    SScConnectionAdamCorr(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,CON_ADAMCORR),
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


class SScConnectionAdam : public SScTrainableParameter
{
public:
    SScConnectionAdam(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,CON_ADAM),
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


class SScConnectionAdamax : public SScTrainableParameter
{
public:
    SScConnectionAdamax(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,CON_ADAM),
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

class SScConnectionAMSGrad : public SScTrainableParameter
{
public:
    SScConnectionAMSGrad(double v, double beta1 = 0.9, double beta2 = 0.99)
        : SScTrainableParameter(v,CON_ADAM),
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
        case CON_STD:       return "Std";       break;
        case CON_RPROP:     return "RProp";     break;
        case CON_RMSPROP:   return "RmsProp";   break;
        case CON_AMSGRAD:   return "AMSGrad";   break;
        case CON_ADAM:      return "Adam";      break;
        case CON_ADAMCORR:  return "AdamCorr";  break;
        case CON_ADAMAX:    return "AdaMax";    break;
        default:                                break;
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
    case CON_STD:       return new SScTrainableParameter(v); break;
    case CON_RPROP:     return new SScConnectionRProp   (v); break;
    case CON_RMSPROP:   return new SScConnectionRMSProp (v); break;
    case CON_AMSGRAD:   return new SScConnectionAMSGrad (v); break;
    case CON_ADAMCORR:  return new SScConnectionAdamCorr(v); break;
    case CON_ADAM:      return new SScConnectionAdam    (v); break;
    case CON_ADAMAX:    return new SScConnectionAdamax  (v); break;
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
    for (int i = (int)CON_STD; i<(int)CON_LAST; ++i)
        if (type2Id((Type)i)==id.toUpper()) return (Type)i;
    ok = false;
    return CON_LAST;
}
