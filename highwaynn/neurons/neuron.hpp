#ifndef HWNEURON_HPP
#define HWNEURON_HPP

#include "gate.hpp"
#include "../nnhelpers/sscactivation.hpp"

#include <QSet>
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
        Conv,
        MaxPool,
        MinPool,
        MedPool,
        Last
    };
    explicit SSiHighwayNeuron(SScHighwayNetwork* net, Type type, SScActivation::Type acttype = SScActivation::IDENTITY);

    virtual ~SSiHighwayNeuron();

    /*!
     * \brief Partial derivative or network error by this output
     * \return
     */
    inline double dedo()
    {
        if (!m_dedoset) { m_dedo = priv_dedo(); m_dedoset = true; }
        return m_dedo;
    }
    virtual double forwardSelectedDedo(SSiHighwayNeuron*) { return 0.0; }
    virtual void    reset   () { m_dedoset = false; m_outset=false;  m_transformset = false; }
    virtual double  err     () { return 0.0; }
    inline double   perr    () { return qPow(err(),2.0); }

    inline  SScActivation* act  () const { return m_act; }
    virtual double icon (SSiHighwayNeuron*) { return 0.00; } //< incoming connection from other neuron
    virtual double ocon (SSiHighwayNeuron* other) { return other->icon(this); }     //< outgoing connection to the other neuron

    virtual bool  setInput(double) = 0;
    virtual bool  setTarget(double) = 0;
    virtual bool setActivation(SScActivation::Type type, double gain = 1.0);
    virtual double deltaw(SSiHighwayNeuron* n) = 0;
    virtual bool addConnection(SSiHighwayNeuron* other, SScTrainableParameter* tp) = 0;
    virtual bool addConnection(SSiHighwayNeuron* other, double v, SScTrainableParameter::Type t) = 0;
    virtual bool delConnection(SSiHighwayNeuron* other) = 0;

    virtual bool addFwdConnection(SSiHighwayNeuron* fwd) { if (!fwd || m_fwd.contains(fwd)) return false; m_fwd << fwd; return true; }
    virtual bool delFwdConnection(SSiHighwayNeuron* fwd) { return m_fwd.remove(fwd); }


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

    virtual void trainingStep   () {}
    virtual void endOfCycle     () {}
    inline void setName(const QString& name) { m_name=name; }
    inline QString name() const { return m_name.isEmpty() ? QString("%1").arg(index()) : m_name; }
    int index() const;

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);

    static QString type2Id(Type t);
    static Type id2Type(const QString& id);
    /*!
     * \brief Called after loading after all neurons are created
     */
    virtual void doConnection() {}
    virtual void dump();

    inline bool conLock     () const { return m_conlock; }
    inline bool gainLock    () const { return m_gainlock; }
    inline void setConLock  (bool b) { m_conlock=b; }
    inline void setGainLock (bool b) { m_gainlock=b; }
    inline void setLock     (bool b) { setConLock(b); setGainLock(b); }

protected:
    // Partial derivative of network error by o_j (with j being the index of this neuron)
    // The same for all neurons with exception of output neurons where it is redefined
    virtual double priv_dedo();

    SScHighwayNetwork*          m_net;
    Type                        m_type;
    bool                        m_conlock,
                                m_gainlock,
                                m_dedoset,
                                m_transformset,
                                m_outset;
    double                      m_dedo, m_t, m_o;
    SScActivation*              m_act;
    QString                     m_name;
    QSet<SSiHighwayNeuron*>     m_fwd;
};

#endif // HWNEURON_HPP
