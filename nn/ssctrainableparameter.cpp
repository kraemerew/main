#include "ssctrainableparameter.hpp"

void SScTrainableParameter::update(double v, bool cycleDone)
{
    m_updatesum += v;
    ++m_ctr;
    if (cycleDone && (m_ctr>0))
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
    SScConnectionRProp(double v) : SScTrainableParameter(v), m_uval(.1), m_last(0) {}
    virtual void update(double v, bool cycleDone)
    {
        m_updatesum += v;
        if (cycleDone)
        {
            const bool signchange = ((m_updatesum>0) && (m_last<0)) || ((m_updatesum<0) && (m_last>0));
            if (signchange) m_uval*=.6; else m_uval*=1.2;
            m_uval=qBound(0.0000001,m_uval,100.0);
            //qWarning(">>>>>>>>>>>RPROP UPDATE %s DLT %lf, V %lf", signchange ?"SC":"SS",m_uval,m_value);
            if (m_updatesum>0) m_value+=m_uval; else if (m_updatesum<0) m_value-=m_uval;
            m_last=m_updatesum;
            m_updatesum=0.0;
        }
    }
private:
    double m_uval, m_last;
};

SScTrainableParameter* SScTrainableParameter::create(Type type, double v)
{
    switch(type)
    {
    case CON_STD: return new SScTrainableParameter(v); break;
    case CON_RPROP: return new SScConnectionRProp(v); break;
    }
    return NULL;
}
