#ifndef SSCNETWORKBASE_HPP
#define SSCNETWORKBASE_HPP
#include "sscactivation.hpp"

class SScNetworkBase
{
public:
    SScNetworkBase();

    bool setOutputActivationType(SScActivation::Type t);
    bool setHiddenActivationType(SScActivation::Type t);
    bool setCarryActivationType (SScActivation::Type t);
    bool setTrainingType(SScTrainableParameter::Type t);

    void setConnectionRange (double v, double interval);
    void setGainRange       (double v, double interval);

    double getRandomConnectionValue () const;
    double getRandomGainValue       () const;

    inline SScActivation::Type hActType() const { return m_htype; }
    inline SScActivation::Type cActType() const { return m_ctype; }
    inline SScActivation::Type oActType() const { return m_otype; }
    inline SScTrainableParameter::Type trainingType() const { return m_tptype; }

private:
    double getRandom(double,double) const;
    double m_cmin, m_cmax, m_gmin, m_gmax;
    SScActivation::Type m_htype, m_ctype, m_otype;
    SScTrainableParameter::Type m_tptype;
};

#endif // SSCNETWORKBASE_HPP
