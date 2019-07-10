#ifndef SSCTPAR_HPP
#define SSCTPAR_HPP
#include <qglobal.h>
#include <QString>
#include <QVariantMap>

class SScTrainableParameter
{
public:
    enum Type
    {
        STD,
        MOMENTUM,
        RPROP,
        RMSPROP,
        AMSGRAD,
        ADAM,
        ADAMCORR,
        ADAMAX,
        ADAMAXCORR,
        LAST
    };

    SScTrainableParameter(double value, SScTrainableParameter::Type t = SScTrainableParameter::RMSPROP) : m_t(t), m_ctr(0), m_eta(.01), m_updatesum(0), m_value(value) {}
    virtual ~SScTrainableParameter() {}
    inline void set(double v) { m_value = v; }
    inline double value() const { return m_value; }
    inline void setEta(double v) { m_eta=qMax(0.000001,v); }
    virtual void update(double v);
    virtual void endOfCycle();
    virtual bool reset() { return false; }
    QString name() const { return name(m_t); }

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);

    static QString  name(Type t);
    static SScTrainableParameter* create(Type type, double v);
    static SScTrainableParameter* create(const QVariantMap&);
    inline SScTrainableParameter::Type trainingType() const { return m_t; }

    static Type id2Type(const QString& id, bool& ok);
    static QString type2Id(Type type);

protected:
    SScTrainableParameter::Type m_t;
    int     m_ctr;
    double  m_eta, m_updatesum, m_value;
};

#endif // SSCTPAR_HPP
