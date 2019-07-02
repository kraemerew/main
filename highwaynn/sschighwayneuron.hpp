#ifndef HWNEURON_HPP
#define HWNEURON_HPP

#include "sschighwaygate.hpp"
#include "../nnhelpers/sscactivation.hpp"

#include <QMap>
#include <QSharedPointer>
#include <QtMath>


class SSiHighwayNeuron
{
public:
    enum SSeNeuronType
    {
        NeuronType_Input,
        NeuronType_Hidden,
        NeuronType_Output,
        NeuronType_Bias
    };
    explicit SSiHighwayNeuron(SSeNeuronType type, SScActivation::Type acttype = SScActivation::ACT_IDENTITY)
        : m_type(type), m_dedoset(false), m_outset(false), m_o(0.0), m_dedo(0.0), m_act(SScActivation::create(acttype)) {}
    virtual ~SSiHighwayNeuron() { if (m_act) delete m_act; m_act=NULL; }
    /*!
     * \brief Partial derivative or network error by this output
     * \return
     */
    inline double dedo()
    {
        if (!m_dedoset) { m_dedo = priv_dedo(); m_dedoset = true; }
        return m_dedo;
    }
    virtual void    reset   () { m_dedoset = false; m_outset=false;  }
    virtual double  err     () { return 0.0; }
    inline double   perr    () { return qPow(err(),2.0); }

    inline  SScActivation* act  () const { return m_act; }
    virtual double icon (SSiHighwayNeuron* other) { Q_UNUSED(other); return 0.00; } //< incoming connection from other neuron
    inline  double ocon (SSiHighwayNeuron* other) { return other->icon(this); }     //< outgoing connection to the other neuron

    virtual bool  setInput(double) = 0;
    virtual bool  setTarget(double) = 0;
    virtual bool setActivation(SScActivation::Type type, double gain = 1.0);
    virtual double deltaw(SSiHighwayNeuron* n) = 0;
    virtual double deltag() = 0;
    virtual bool addInput(SSiHighwayNeuron* other, double v) = 0;
    virtual bool delInput(SSiHighwayNeuron* other) = 0;
    virtual double net() { return 0.0; }
    virtual double out() { return 0.0; }
    virtual double carry() = 0;
    virtual double highway() = 0;
    virtual bool connectHighway(SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn) = 0;

    virtual QList<SSiHighwayNeuron*> inputs() const { return QList<SSiHighwayNeuron*>(); }
    virtual QList<SSiHighwayNeuron*> allInputs() const { return inputs(); }

    static SSiHighwayNeuron* create(SSeNeuronType type, const QString& name = QString());
    virtual void connectForward (const QList<SSiHighwayNeuron*>& fwd) { m_out = fwd; }
    virtual void trainingStep   () {}
    virtual void endOfCycle     () {}
    inline void setName(const QString& name) { m_name=name; }
    inline QString name() const { return m_name; }

protected:
    // Partial derivative of network error by o_j (with j being the index of this neuron)
    // The same for all neurons with exception of output neurons where it is redefined
    virtual double priv_dedo();

    SSeNeuronType               m_type;
    bool                        m_dedoset, m_outset;
    double                      m_o, m_dedo;
    SScActivation*              m_act;
    QString                     m_name;
    QList<SSiHighwayNeuron*>    m_out;
};

#endif // HWNEURON_HPP
