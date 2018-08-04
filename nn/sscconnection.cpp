#include "sscconnection.hpp"
#include <QtMath>

class SScConnectionSimple : public SScConnection
{
public:
    SScConnectionSimple(double v, double initmin, double initmax, double eta) : SScConnection(ConnectionType_Simple,v,initmin,initmax,eta) {}
};

class SScConnectionRPROP : public SScConnection
{
public:
    SScConnectionRPROP(double v, double initmin, double initmax, double eta, double ep= 1.2, double em = 0.6)
        : SScConnection(Connectiontype_RPROP,v,initmin,initmax,eta), m_ep(ep), m_em(em)
    {}
    double m_ep, m_em;
};
class SScConnectionLPEM : public SScConnection
{
public:
    SScConnectionLPEM(double v, double initmin, double initmax, double eta, double lp = 0.001, double em = 0.6)
        : SScConnection(Connectiontype_LPEM,v,initmin,initmax,eta), m_lp(lp), m_em(em)
    {}
    double m_lp, m_em;
};

void SScConnection::init()
{
    m_v = 0.5*tanh(qrand())*(m_initmax-m_initmin)+m_initmin;
}

SScConnection::SScConnection(SSeConnectionType type, double value, double initmin, double initmax, double eta)
    : m_type(type), m_v(value), m_initmin(initmin), m_initmax(initmax), m_eta(eta)
{}

SScConnection SScConnection::create(SSeConnectionType type, bool& ok, double v, double initmin, double initmax, double eta)
{
    ok = true;
    switch(type)
    {
    case ConnectionType_Simple: return SScConnectionSimple(v,initmin,initmax,eta); break;
    case Connectiontype_RPROP:  return SScConnectionRPROP (v,initmin,initmax,eta); break;
    case Connectiontype_LPEM:   return SScConnectionLPEM  (v,initmin,initmax,eta); break;
    }
    ok = false;
    return SScConnectionSimple(v,initmin,initmax,eta);
}
