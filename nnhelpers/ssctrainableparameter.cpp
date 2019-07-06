#include "ssctrainableparameter.hpp"
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



class SScConnectionRProp : public SScTrainableParameter
{
public:
    SScConnectionRProp(double v) : SScTrainableParameter(v,CON_RPROP), m_uval(.1), m_last(0) {}
    virtual void endOfCycle()
    {
        if (m_ctr>0)
        {
            const bool signchange = ((m_updatesum>0) && (m_last<0)) || ((m_updatesum<0) && (m_last>0));
            if (signchange) m_uval*=.6; else m_uval*=1.2;
            m_uval=qBound(0.00000001,m_uval,100.0);
            if (m_updatesum>0) m_value+=m_uval; else if (m_updatesum<0) m_value-=m_uval;
            m_last=m_updatesum;
            m_updatesum=0.0;
            m_ctr = 0;
        }
    }

private:
    double m_uval, m_last;
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
    case CON_ADAM:      return "Adam";      break;
    case CON_ADAMCORR:  return "AdamCorr";  break;
    }
    return "";
}

SScTrainableParameter* SScTrainableParameter::create(Type type, double v)
{
    switch(type)
    {
    case CON_STD:       return new SScTrainableParameter(v); break;
    case CON_RPROP:     return new SScConnectionRProp   (v); break;
    case CON_RMSPROP:   return new SScConnectionRMSProp (v); break;
    case CON_ADAMCORR:  return new SScConnectionAdamCorr(v); break;
    case CON_ADAM:      return new SScConnectionAdam    (v); break;
    }
    return NULL;
}
