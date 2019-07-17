#ifndef SSCACTIVATION_HPP
#define SSCACTIVATION_HPP

#include <QString>
#include <QVariantMap>
#include "ssctrainableparameter.hpp"

class SScActivation
{
public:
    enum Type
    {
        IDENTITY,
        RELU,
        LOGISTIC,
        TANH,
        RBF,
        SOFTPLUS,
        SWISH,
        MHAT,
        GDER,
        X,
        LAST
    };

    SScActivation(Type t)
        : m_t   (t),
          m_pot (0),
          m_act (0),
          m_gain(SScTrainableParameter::create(SScTrainableParameter::ADAM,1.0))
    {}

    virtual ~SScActivation() { delete m_gain; }
    virtual QString name() const { return name(m_t); }

    void setTrainingType(SScTrainableParameter::Type t);
    inline double activate  (double pot) { m_pot = pot; priv_activate(); return m_act; }
    inline double dev       () { return priv_dev(); }
    inline double gain      () const { return m_gain->value(); }
    inline bool   setGain   (double v) { m_gain->set(v); return true; }
    inline void   endOfCycle() { m_gain->endOfCycle(); }
    inline QString type2Id() const { return type2Id(m_t); }

    static SScActivation* create(Type type);
    static SScActivation* create(const QVariantMap& vm);
    static QString name(Type type);
    static QString type2Id(Type type);
    static Type id2Type(const QString& id, bool& ok);
    static bool canCarry(Type type);
    static bool nonLinear(Type type);
    /*!
     * \brief Whether this activation can be used for a carry signal (range [0;1])
     * \return
     */
    virtual bool canCarry() const { return canCarry(m_t); }
    virtual bool nonLinear() const { return nonLinear(m_t); }

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);

    virtual void update(double dedo, double carry)
    {
        update(-dedo*dev()*m_pot*(1.0-carry));
    }

protected:
    inline void   update(double v) { m_gain->update(v); }

    virtual void priv_activate() = 0;
    virtual double priv_dev() = 0;
    Type m_t;
    double m_pot, m_act;
    SScTrainableParameter* m_gain;
};

#endif // SSCACTIVATION_HPP
