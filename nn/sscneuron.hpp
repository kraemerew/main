#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP
#include <QList>
#include <QtMath>
#include <QDebug>
class SScConnection
{
public:
    enum SSeConnection
    {
        CON_STD,
        CON_RPROP
    };

    SScConnection(double value) : m_ctr(0), m_eta(.1), m_updatesum(0), m_value(value) {}
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
    enum SSeActivation
    {
        ACT_IDENTITY,
        ACT_SIGMOID,
        ACT_TANH,
        ACT_RBF,
        ACT_SOFTPLUS,
        ACT_MHAT,
        ACT_GDER,
        ACT_X
    };

    SScActivation()
        : m_pot (0),
          m_act (0),
          m_gain(SScConnection::create(SScConnection::CON_RPROP,1.0))
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
    SScConnection* m_gain;
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
    inline  double perr() { return qPow(err(),2.0); }
    virtual double err() { return 0.0; }
    virtual double out() = 0;
    virtual double net() = 0;
    /*!
     * \brief Partial derivative or network error by this output
     * \return
     */
    inline double dedo()
    {
        if (!m_dedoset) { m_dedo = priv_dedo(); m_dedoset = true; }
        return m_dedo;
    }
    inline void resetDedo()
    {
        m_dedoset = false;
    }

    inline  SScActivation* act  () const { return m_act; }
    virtual double icon (SScNeuron* other) { Q_UNUSED(other); return 0.0; }               //< incoming connection from other neuron
    inline  double ocon (SScNeuron* other) { return other->icon(this); }  //< outgoing connection to the other neuron

    virtual bool  setInput(double) = 0;
    virtual bool  setTarget(double) = 0;
    virtual bool setActivation(SScActivation::SSeActivation type);
    virtual double deltaw(SScNeuron* n) = 0;
    virtual double deltag() = 0;

    virtual QList<SScNeuron*> inputs() const = 0;
    static SScNeuron* create(SSeNeuronType type, const QString& name = QString());
    inline SSeNeuronType type() const { return m_type; }
    virtual void connectForward(const QList<SScNeuron*>& fwd) { m_out = fwd; }
    virtual bool trainingStep(bool cycleDone) = 0;

    inline void setName(const QString& name) { m_name=name; }
    inline QString name() const { return m_name; }

protected:
    // Partial derivative of network error by o_j (with j being the index of this neuron)
    // The same for all neurons with exception of output neurons where it is redefined
    virtual double priv_dedo()
    {       
        // dE/do_j = sum(l) dE/dnet_l * dnet_l/do_j = sum(l) dE/dnet_l wjl
        //         = sum(l) w_jl dE/do_l do_l/dnet_l
        //         = sum(l) w_jl dedo(l) act(l)'gain_l
        double ret = 0;

        foreach(SScNeuron* l, m_out)
        {
            const double w_jl = l->icon(this);
            ret += l->dedo()*w_jl*l->act()->dev()*l->act()->gain();
        }
        return ret;
    }

    SSeNeuronType       m_type;
    bool                m_dedoset;
    double              m_dedo;
    SScActivation*      m_act;
    QString             m_name;
    QList<SScNeuron*>   m_out;
};
#endif // SSCNEURON_HPP
