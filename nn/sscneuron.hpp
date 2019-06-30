#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP
#include <QList>
#include <QtMath>
#include <QDebug>
#include "ssineuron.hpp"
#include "../nnhelpers/ssctrainableparameter.hpp"
#include "../nnhelpers/sscactivation.hpp"

class SScNeuron : public SSiNeuron
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
    /*!
     * \brief Partial derivative or network error by this output
     * \return
     */
    inline double dedo()
    {
        if (!m_dedoset) { m_dedo = priv_dedo(); m_dedoset = true; }
        return m_dedo;
    }
    virtual void reset()
    {
        m_dedoset = false;
    }

    inline  SScActivation* act  () const { return m_act; }
    virtual double icon (SScNeuron* other) { Q_UNUSED(other); return 0.0; }               //< incoming connection from other neuron
    inline  double ocon (SScNeuron* other) { return other->icon(this); }  //< outgoing connection to the other neuron

    virtual bool  setInput(double) = 0;
    virtual bool  setTarget(double) = 0;
    virtual bool setActivation(SScActivation::Type type);
    virtual double deltaw(SScNeuron* n) = 0;
    virtual double deltag() = 0;

    virtual bool    addInputC(SScNeuron*,double) { return false; }
    virtual bool    delInputC(SScNeuron*) { return false; }

    virtual QList<SScNeuron*> inputs() const = 0;

    static SScNeuron* create(SSeNeuronType type, const QString& name = QString());
    inline SSeNeuronType type() const { return m_type; }
    virtual void connectForward(const QList<SScNeuron*>& fwd) { m_out = fwd; }
    virtual bool trainingStep() { return false; }
    virtual bool endOfCycle() { return false; }
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
