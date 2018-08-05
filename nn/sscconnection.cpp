#include "sscconnection.hpp"
#include <QtMath>

class SScConnectionPrivate
{
   public:
    explicit SScConnectionPrivate(SScConnection::SSeConnectionType type, double v, double initmin, double initmax, double eta)
        : m_type(type), m_v(v), m_initmin(initmin), m_initmax(initmax), m_eta(eta), m_sumdlt(0)
    {
    }
    virtual ~SScConnectionPrivate() {}
    virtual void trainingReset() { m_sumdlt = 0; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_sumdlt+=dlt;
        if (cycleDone)
        {
            m_v += m_eta*m_sumdlt;
            m_sumdlt = 0;
        }
    }
    virtual void init() {     m_v = 0.5*tanh(qrand())*(m_initmax-m_initmin)+m_initmin; }
    SScConnection::SSeConnectionType m_type;
    double value() const { return m_v; }
    double eta() const { return m_eta; }
    double m_v, m_initmin, m_initmax, m_eta, m_sumdlt = 0;
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
    virtual void trainingReset() { m_lastdlt = 0; m_sumdlt = 0; }
    virtual void update(double dlt, bool cycleDone)
    {
        m_sumdlt+=dlt;
        if (cycleDone)
        {
            const double mdlt = m_sumdlt+(m_m*m_lastdlt);
            m_lastdlt = mdlt;
            m_v += (m_eta*mdlt);
            m_sumdlt = 0;
        }
    };

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
        m_sumdlt += dlt;
        if (cycleDone)
        {
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*m_sumdlt>0) m_eta*=m_ep; else m_eta*=m_em;
                m_eta = qBound(0.001,m_eta,1000.0);
            }
            m_v += (m_eta*m_sumdlt);
            m_lastdlt = m_sumdlt;
            m_sumdlt = 0;
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
        m_sumdlt += dlt;
        if (cycleDone)
        {
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*m_sumdlt>0) m_eta+=m_lp; else m_eta*=m_em;
                m_eta = qBound(0.001,m_eta,100.0);
            }
            m_v += (m_eta*m_sumdlt);
            m_lastdlt = m_sumdlt;
            m_sumdlt = 0;
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
        m_sumdlt += dlt;
        if (cycleDone)
        {
            if (Q_LIKELY(m_lastdlt!=0))
            {
                if (m_lastdlt*m_sumdlt>0) m_eta+=m_lp; else m_eta-=m_lm;
                m_eta = qBound(0.001,m_eta,100.0);
            }
            m_v += (m_eta*m_sumdlt);
            m_lastdlt = m_sumdlt;
            m_sumdlt = 0;
        }
    }

    double m_lp, m_lm, m_lastdlt, m_etainit;
};






SScConnection::SScConnection(SSeConnectionType type, double v, double initmin, double initmax, double eta) : d_ptr(NULL)
{
    switch(type)
    {
    case SScConnection::ConnectionType_Simple:      d_ptr = new (std::nothrow) SScConnectionSimple  (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_Momentum:    d_ptr = new (std::nothrow) SScConnectionMomentum(v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_RPROP:       d_ptr = new (std::nothrow) SScConnectionRPROP   (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_LPEM:        d_ptr = new (std::nothrow) SScConnectionLPEM    (v,initmin,initmax,eta); break;
    case SScConnection::Connectiontype_LPLM:        d_ptr = new (std::nothrow) SScConnectionLPLM    (v,initmin,initmax,eta); break;
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
