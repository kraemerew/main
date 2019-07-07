#include "sscnetworkbase.hpp"
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

bool    SScNetworkBase::setTrainingType(SScTrainableParameter::Type t) { m_tptype=t; }
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
