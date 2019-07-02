#ifndef SSCTPAR_HPP
#define SSCTPAR_HPP
#include <qglobal.h>

class SScTrainableParameter
{
public:
    enum Type
    {
        CON_STD,
        CON_RPROP
    };

    SScTrainableParameter(double value) : m_ctr(0), m_eta(.1), m_updatesum(0), m_value(value) {}
    virtual ~SScTrainableParameter() {}
    inline void set(double v) { m_value = v; }
    inline double value() const { return m_value; }
    inline void setEta(double v) { m_eta=qMax(0.000001,v); }
    virtual void update(double v);
    virtual void endOfCycle();
    static SScTrainableParameter* create(Type type, double v);

protected:
    int     m_ctr;
    double  m_eta, m_updatesum, m_value;
};


#endif // SSCTPAR_HPP
