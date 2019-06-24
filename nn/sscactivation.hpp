#ifndef SSCACTIVATION_HPP
#define SSCACTIVATION_HPP

#include <QString>
#include "ssctrainableparameter.hpp"

class SScActivation
{
public:
    enum SSeActivation
    {
        ACT_IDENTITY,
        ACT_SIGMOID,
        ACT_TANH,
        ACT_RBF,
        ACT_SOFTPLUS,
        ACT_SWISH,
        ACT_MHAT,
        ACT_GDER,
        ACT_X
    };

    SScActivation()
        : m_pot (0),
          m_act (0),
          m_gain(SScTrainableParameter::create(SScTrainableParameter::CON_RPROP,1.0))
    {}
    virtual ~SScActivation() { delete m_gain; }
    virtual QString name() const = 0;

    inline double activate  (double pot) { m_pot = pot; priv_activate(); return m_act; }
    inline double dev       () { return priv_dev(); }
    inline double gain      () const { return m_gain->value(); }
    inline void   updateGain(double v, bool cycleDone) { m_gain->update(v,cycleDone); }
    static SScActivation* create(SSeActivation type);

protected:
    virtual void priv_activate() = 0;
    virtual double priv_dev() = 0;
    double m_pot, m_act;
    SScTrainableParameter* m_gain;
};

#endif // SSCACTIVATION_HPP
