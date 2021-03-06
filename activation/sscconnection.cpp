#include "sscconnection.hpp"
#include <QtMath>

class SScAccumulator
{
    public:
    SScAccumulator() : m_acc(0), m_ctr(0) {}
    void clear() { m_ctr = 0; m_acc=0; }
    double get()
    {
        const double ret = (m_ctr>0) ? m_acc/(double)m_ctr : 0.0;
        clear();
        return ret;
    }
    void operator << (double v) { m_acc+=v; ++m_ctr; }

private:
    double  m_acc;
    quint32 m_ctr;
};

class SScConnectionPrivate
{
   public:
    explicit SScConnectionPrivate(SScConnection::SSeConnectionType type, double v, double initmin, double initmax, double eta)
        : m_type(type), m_v(v), m_initmin(initmin), m_initmax(initmax), m_eta(eta)
    {
    }
    virtual ~SScConnectionPrivate() {}
    virtual void trainingReset() { m_dltacc.clear(); }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone) m_v += m_eta*m_dltacc.get();
    }
    virtual void init() {     m_v = 0.5*tanh(qrand())*(m_initmax-m_initmin)+m_initmin; }
    SScConnection::SSeConnectionType m_type;
    double value() const { return m_v; }
    double eta() const { return m_eta; }
    double m_v, m_initmin, m_initmax, m_eta;
    SScAccumulator  m_dltacc;
};

class SScConnectionSimple : public SScConnectionPrivate
{
public:
    SScConnectionSimple(double v, double initmin, double initmax, double eta) : SScConnectionPrivate(SScConnection::ConnectionType_Simple,v,initmin,initmax,eta) {}
    virtual void update(double dlt) { m_v += (m_eta*dlt); }
};
class SScConnectionMomentum : public SScConnectionPrivate
{
public:
    SScConnectionMomentum(double v, double initmin, double initmax, double eta, double m = 0.5)
        : SScConnectionPrivate(SScConnection::Connectiontype_Momentum,v,initmin,initmax,eta), m_m(m), m_lastdlt(0)
    {}
    virtual void trainingReset() { m_lastdlt = 0; m_dltacc.clear(); }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double mdlt = m_dltacc.get()+(m_m*m_lastdlt);
            m_lastdlt = mdlt;
            m_v += (m_eta*mdlt);
         }
    }

    double m_m, m_lastdlt;
};
class SScConnectionRPROP : public SScConnectionPrivate
{
public:
    SScConnectionRPROP(double v, double initmin, double initmax, double eta, double ep= 1.2, double em = 0.6)
        : SScConnectionPrivate(SScConnection::Connectiontype_RPROP,v,initmin,initmax,eta), m_ep(ep), m_em(em), m_lastdlt(0), m_etainit(eta)
    {}
    virtual void trainingReset() { m_lastdlt = 0; m_eta = m_etainit; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*meandlt>0) m_eta*=m_ep; else m_eta*=m_em;
                m_eta = qBound(0.0001,m_eta,0.1);
            }
            if (meandlt>0) m_v += m_eta; else m_v -= m_eta;
            m_lastdlt = meandlt;
        }
    }

    double m_ep, m_em, m_lastdlt, m_etainit;
};



class SScConnectionEPEM : public SScConnectionPrivate
{
public:
    SScConnectionEPEM(double v, double initmin, double initmax, double eta, double ep = 1.2, double em = 0.6)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta), m_ep(ep), m_em(em), m_lastdlt(0), m_etainit(eta)
    {}
    virtual void trainingReset() { m_lastdlt = 0; m_eta = m_etainit; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*meandlt>0) m_eta*=m_ep; else m_eta*=m_em;
                m_eta = qBound(0.001,m_eta,1000.0);
            }
            m_v += (m_eta*meandlt);
            m_lastdlt = meandlt;
        }
    }

    double m_ep, m_em, m_lastdlt, m_etainit;
};

class SScConnectionLPEM : public SScConnectionPrivate
{
public:
    SScConnectionLPEM(double v, double initmin, double initmax, double eta, double lp = 0.1, double em = 0.6)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta), m_lp(lp), m_em(em), m_lastdlt(0), m_etainit(eta)
    {}
    virtual void trainingReset() { m_lastdlt = 0; m_eta = m_etainit; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*meandlt>0) m_eta+=m_lp; else m_eta*=m_em;
                m_eta = qBound(0.001,m_eta,1000.0);
            }
            m_v += (m_eta*meandlt);
            m_lastdlt = meandlt;
        }
    }

    double m_lp, m_em, m_lastdlt, m_etainit;
};
class SScConnectionLPLM : public SScConnectionPrivate
{
public:
    SScConnectionLPLM(double v, double initmin, double initmax, double eta, double lp = 0.1, double lm = 0.5)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta), m_lp(lp), m_lm(lm), m_lastdlt(0), m_etainit(eta)
    {}
    virtual void trainingReset() { m_lastdlt = 0; m_eta = m_etainit; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*meandlt>0) m_eta+=m_lp; else m_eta-=m_lm;
                m_eta = qBound(0.001,m_eta,1000.0);
            }
            m_v += (m_eta*meandlt);
            m_lastdlt = meandlt;
        }
    }

    double m_lp, m_lm, m_lastdlt, m_etainit;
};


class SScConnectionRMSProp: public SScConnectionPrivate
{
public:
    SScConnectionRMSProp(double v, double initmin, double initmax, double eta, double beta = 0.9, double epsilon = 1e-6)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta),
          m_beta(beta), m_epsilon(epsilon), m_secorder(0)
    {}
    virtual void trainingReset() { m_secorder = 0; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            m_secorder = (m_beta*m_secorder)  +((1.0-m_beta)*(meandlt*meandlt));
            m_v += (m_eta*meandlt)/qSqrt(m_epsilon+m_secorder);
         }
    }

    double  m_beta, m_epsilon, m_secorder;
};

class SScConnectionAdam: public SScConnectionPrivate
{
public:
    SScConnectionAdam(double v, double initmin, double initmax, double eta, double beta1 = 0.9, double beta2 = 0.999, double epsilon = 1e-6)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta),
          m_beta1(beta1), m_beta2(beta2), m_epsilon(epsilon), m_firstorder(0), m_secorder(0), m_beta1power(1), m_beta2power(1)
    {}
    virtual void trainingReset() {
        m_firstorder = 0, m_secorder = 0; m_beta1power = 1; m_beta2power=1; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            m_beta1power *= m_beta1;
            m_beta2power *= m_beta2;

            m_firstorder = (m_beta1*m_firstorder)+((1.0-m_beta1)*meandlt);
            m_secorder   = (m_beta2*m_secorder)  +((1.0-m_beta2)*(meandlt*meandlt));

            const double mcorr = m_firstorder / (1.0-m_beta1power),
                         vcorr = m_secorder   / (1.0-m_beta2power);
            m_v+= (m_eta*mcorr)/(m_epsilon+qSqrt(vcorr));
        }
    }

    double  m_beta1, m_beta2, m_epsilon, m_firstorder, m_secorder, m_beta1power, m_beta2power;
};

class SScConnectionAMSGrad: public SScConnectionPrivate
{
public:
    SScConnectionAMSGrad(double v, double initmin, double initmax, double eta, double beta1 = 0.9, double beta2 = 0.999, double epsilon = 1e-6)
        : SScConnectionPrivate(SScConnection::Connectiontype_LPEM,v,initmin,initmax,eta),
          m_beta1(beta1), m_beta2(beta2), m_epsilon(epsilon), m_firstorder(0), m_secorder(0), m_secordercorr(0)
    {}
    virtual void trainingReset() { m_firstorder = 0, m_secorder = 0; m_secordercorr=0; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_dltacc << dlt;
        if (cycleDone)
        {
            const double meandlt = m_dltacc.get();
            m_firstorder = (m_beta1*m_firstorder)+((1.0-m_beta1)*meandlt);
            m_secorder   = (m_beta2*m_secorder)  +((1.0-m_beta2)*(meandlt*meandlt));

            m_secordercorr = qMax(m_secorder,m_secordercorr);
            m_v+= (m_eta*m_firstorder)/(m_epsilon+qSqrt(m_secordercorr));
        }
    }

    double  m_beta1, m_beta2, m_epsilon, m_firstorder, m_secorder, m_secordercorr;
};




SScConnection::SScConnection(SSeConnectionType type, double v, double initmin, double initmax, double eta) : d_ptr(NULL)
{
    switch(type)
    {
    case SScConnection::ConnectionType_Simple:      d_ptr = new (std::nothrow) SScConnectionSimple  (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_Momentum:    d_ptr = new (std::nothrow) SScConnectionMomentum(v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_RPROP:       d_ptr = new (std::nothrow) SScConnectionRPROP   (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_EPEM:        d_ptr = new (std::nothrow) SScConnectionEPEM    (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_LPEM:        d_ptr = new (std::nothrow) SScConnectionLPEM    (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_LPLM:        d_ptr = new (std::nothrow) SScConnectionLPLM    (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_RMSProp:     d_ptr = new (std::nothrow) SScConnectionRMSProp (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_Adam:        d_ptr = new (std::nothrow) SScConnectionAdam    (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_AMSGrad:     d_ptr = new (std::nothrow) SScConnectionAMSGrad (v,initmin,initmax,eta); break;
    }
    Q_CHECK_PTR(d_ptr);
    if (!d_ptr) d_ptr = new (std::nothrow) SScConnectionSimple(v,initmin,initmax,eta);
}

SScConnection::~SScConnection() { delete d_ptr; }

SScConnection::SSeConnectionType    SScConnection::type         () const                        { return d_ptr->m_type; }
void                                SScConnection::init         ()                              { d_ptr->init(); }
void                                SScConnection::update       (double dlt, bool cycleDone)    { d_ptr->update(dlt,cycleDone); }
double                              SScConnection::value        () const                        { return d_ptr->value(); }
void                                SScConnection::trainingReset()                              { d_ptr->trainingReset(); }
