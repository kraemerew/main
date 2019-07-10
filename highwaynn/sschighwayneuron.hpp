#ifndef HWNEURON_HPP
#define HWNEURON_HPP

#include "sschighwaygate.hpp"
#include "../nnhelpers/sscactivation.hpp"

#include <QMap>
#include <QSharedPointer>
#include <QtMath>
class SScHighwayNetwork;

class SSiHighwayNeuron
{
public:
    enum Type
    {
        Input,
        Hidden,
        Output,
        Bias,
        Carry,
        Last
    };
    explicit SSiHighwayNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype = SScActivation::IDENTITY)
        : m_net(net),
          m_type(type),
          m_dedoset(false),
          m_transformset(false),
          m_outset(false),
          m_dedo(0.0),
          m_t(0.0),
          m_o(0.0),
          m_act(SScActivation::create(acttype))
    {
       Q_CHECK_PTR(net);
        reset();
    }
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
    virtual void    reset   () { m_dedoset = false; m_outset=false;  m_transformset = false; }
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
    virtual bool addInput(SSiHighwayNeuron* other, SScTrainableParameter* tp) = 0;
    virtual bool addInput(SSiHighwayNeuron* other, double v, SScTrainableParameter::Type t) = 0;
    virtual bool delInput(SSiHighwayNeuron* other) = 0;
    virtual double net() { return 0.0; }
    virtual double out() { return 0.0; }
    virtual double transform() { return 0.0; }
    virtual double carry() { return 0.0; }
    virtual double highway() { return 0.0; }
    virtual double highwayMinusTransform() { return highway()-transform(); }
    virtual bool connectHighway(SSiHighwayNeuron* hwn, SSiHighwayNeuron* cn) = 0;

    virtual QList<SSiHighwayNeuron*> inputs() const { return QList<SSiHighwayNeuron*>(); }
    virtual QList<SSiHighwayNeuron*> allInputs() const { return inputs(); }
    static SSiHighwayNeuron* create(SScHighwayNetwork* net, const QVariantMap&);
    static SSiHighwayNeuron* create(SScHighwayNetwork* net, Type type, const QString& name = QString());
    virtual void connectForward (const QList<SSiHighwayNeuron*>& fwd) { qWarning("N %s FORWARD %d", qPrintable(name()), fwd.size()); m_out = fwd; }
    virtual void trainingStep   () {}
    virtual void endOfCycle     () {}
    inline void setName(const QString& name) { m_name=name; }
    inline QString name() const { return m_name; }
    int index();

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);

    static QString type2Id(Type t);
    static Type id2Type(const QString& id);
    /*!
     * \brief Called after loading after all neurons are created
     */
    virtual void doConnection() {}

protected:
    // Partial derivative of network error by o_j (with j being the index of this neuron)
    // The same for all neurons with exception of output neurons where it is redefined
    virtual double priv_dedo();

    SScHighwayNetwork*          m_net;
    Type                        m_type;
    bool                        m_dedoset, m_transformset, m_outset;
    double                      m_dedo, m_t, m_o;
    SScActivation*              m_act;
    QString                     m_name;
    QList<SSiHighwayNeuron*>    m_out;
};

#endif // HWNEURON_HPP
