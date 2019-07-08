#include "sscnetworkbase.hpp"
#include "../nnhelpers/sscvm.hpp"
#include <QtMath>

SScNetworkBase::SScNetworkBase() :
    m_cmin  (-1),
    m_cmax  ( 1),
    m_gmin  (-1),
    m_gmax  ( 1),
    m_htype (SScActivation::ACT_TANH),
    m_ctype (SScActivation::ACT_SIGMOID),
    m_otype (SScActivation::ACT_SWISH),
    m_tptype(SScTrainableParameter::CON_ADAM)
{}

bool    SScNetworkBase::setTrainingType(SScTrainableParameter::Type t) { m_tptype=t; return true; }
bool    SScNetworkBase::setOutputActivationType(SScActivation::Type t) { if (!SScActivation::nonLinear(t)) return false; m_htype = t; return true; }
bool    SScNetworkBase::setHiddenActivationType(SScActivation::Type t) { if (!SScActivation::nonLinear(t)) return false; m_htype = t; return true; }
bool    SScNetworkBase::setCarryActivationType (SScActivation::Type t) { if (!SScActivation::canCarry (t)) return false; m_ctype = t; return true; }

void    SScNetworkBase::setConnectionRange          (double v, double interval) { interval=qAbs(interval); m_cmin=v-interval; m_cmax=v+interval; }
void    SScNetworkBase::setGainRange                (double v, double interval) { interval=qAbs(interval); m_gmin=v-interval; m_gmax=v+interval; }
double  SScNetworkBase::getRandomConnectionValue    () const                    { return getRandom(m_cmin, m_cmax); }
double  SScNetworkBase::getRandomGainValue          () const                    { return getRandom(m_gmin, m_gmax); }

double  SScNetworkBase::getRandom(double min, double max) const
{
    Q_ASSERT(min<=max);
    double ret = qrand();
    ret = ret/(double)RAND_MAX;
    ret*=(max-min);
    ret+=min;
    return ret;
}


QVariantMap SScNetworkBase::toVM() const
{
    QVariantMap vm;
    vm["GAIN_MIN"] = m_gmin;
    vm["GAIN_MAX"] = m_gmax;
    vm["CON_MIN"]  = m_cmin;
    vm["CON_MAX"]  = m_cmax;
    vm["ACTTYPE_H"]= SScActivation::type2Id         (m_htype);
    vm["ACTTYPE_O"]= SScActivation::type2Id         (m_otype);
    vm["ACTTYPE_C"]= SScActivation::type2Id         (m_ctype);
    vm["TTYPE"]    = SScTrainableParameter::type2Id (m_tptype);
    return vm;
}
bool SScNetworkBase::fromVM(const QVariantMap& vm)
{
    SScVM sscvm(vm);
    m_gmin = sscvm.doubleToken("GAIN_MIN",m_gmin);
    m_gmax = sscvm.doubleToken("GAIN_MAX",m_gmax);
    m_cmin = sscvm.doubleToken("CON_MIN", m_cmin);
    m_cmax = sscvm.doubleToken("CON_MAC", m_cmax);
    bool ok1, ok2, ok3, ok4;
    m_htype = SScActivation::id2Type(sscvm.stringToken("ACTTYPE_H",SScActivation::type2Id(m_htype)),ok1);
    m_otype = SScActivation::id2Type(sscvm.stringToken("ACTTYPE_O",SScActivation::type2Id(m_otype)),ok2);
    m_ctype = SScActivation::id2Type(sscvm.stringToken("ACTTYPE_C",SScActivation::type2Id(m_ctype)),ok3);
    m_tptype= SScTrainableParameter::id2Type(sscvm.stringToken("TTYPE",SScTrainableParameter::type2Id(m_tptype)),ok4);
    return ok1 && ok2 && ok3 && ok4;
}
