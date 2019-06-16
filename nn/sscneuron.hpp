#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP
#include <QList>

class SScConnection
{
public:
    enum SSeConnection
    {
        CON_STD,
        CON_RPROP
    };

    SScConnection(double value) : m_ctr(0), m_eta(.01), m_updatesum(0), m_value(value) {}
    virtual ~SScConnection() {}
    inline double value() const { return m_value; }
    inline void setEta(double v) { m_eta=qMax(0.000001,v); }
    virtual void update(double v, bool cycleDone)
    {
        m_updatesum += v;
        ++m_ctr;
        if (cycleDone && (m_ctr>0))
        {            
            const double dlt = m_eta*(m_updatesum/(double)m_ctr);
            m_value+=dlt;
            m_updatesum=0.0;
            m_ctr = 0;
        }
    }

    static SScConnection* create(SSeConnection type, double v);

protected:
    int     m_ctr;
    double  m_eta, m_updatesum, m_value;
};


class SScActivation
{
public:
    SScActivation() : m_pot(0),m_act(0) {}
    virtual ~SScActivation() {}
    enum SSeActivation
    {
        ACT_IDENTITY,
        ACT_SIGMOID,
        ACT_TANH
    };
    virtual QString name() const = 0;
    double activate(double pot) { m_pot = pot; priv_activate(); return m_act; }
    double dev() { return priv_dev(); }
    static SScActivation* create(SSeActivation type);
protected:
    virtual void priv_activate() = 0;
    virtual double priv_dev() = 0;
    double m_pot, m_act;
};

class SScNeuron
{
public:
enum SSeNeuronType
{
    NeuronType_Input,
    NeuronType_Hidden,
    NeuronType_Output,
    NeuronType_Bias
};
    SScNeuron(SSeNeuronType type);
    virtual ~SScNeuron();
    virtual bool addInput(SScNeuron* other, double v) = 0;
    virtual bool delInput(SScNeuron* other) = 0;
    virtual double err() { return 0.0; }
    virtual double out() = 0;
    virtual double net() = 0;
    virtual double dlt() = 0;
    virtual double dltFwd(SScNeuron* n) = 0;
    virtual bool  setInput(double) = 0;
    virtual bool  setTarget(double) = 0;
    virtual bool setActivation(SScActivation::SSeActivation type);
    virtual double deltaw(SScNeuron* n) = 0;
    virtual QList<SScNeuron*> inputs() const = 0;
    static SScNeuron* create(SSeNeuronType type, const QString& name = QString());
    inline SSeNeuronType type() const { return m_type; }
    virtual void connectForward(const QList<SScNeuron*>& fwd) = 0;
    virtual bool trainingStep(bool cycleDone) = 0;

    void setName(const QString& name) { m_name=name; }
    inline QString name() const { return m_name; }

protected:
    SSeNeuronType   m_type;
    SScActivation*  m_act;
    QString         m_name;

};
#endif // SSCNEURON_HPP
