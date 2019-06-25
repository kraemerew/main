#ifndef HWNEURON_HPP
#define HWNEURON_HPP

#include "sscneuron.hpp"
#include "sscgate.hpp"
#include "sscactivation.hpp"

#include <QMap>
#include <QSharedPointer>


class SScHighwayNetwork;
class SScHighwayNeuron : public SScNeuron
{
public:
    /*!
     * \brief Constructor
     * \param nw    Containing network
     * \param layer Layer
     * \param nr    Index in layer
     */
    explicit SScHighwayNeuron(SScHighwayNetwork* nw, int layer, int nr);
    virtual ~SScHighwayNeuron();

    virtual bool    setInput(double) { Q_ASSERT(false); return false; }
    virtual bool    setTarget(double) { Q_ASSERT(false); return false; }
    virtual void    reset() {}
    virtual bool    addInput(SScNeuron* other, double v) { return m_in.addInput(other,v); }
    virtual bool    delInput(SScNeuron* other) { return m_in.delInput(other); }
    virtual bool    addInputC(SScNeuron* other, double v) { return m_cin.addInput(other,v); }
    virtual bool    delInputC(SScNeuron* other) { return m_cin.delInput(other); }
    virtual double  net() { return m_in.net(); }
    virtual double  netC() { return m_cin.net(); }

    virtual double deltaw(SScNeuron*) { return 0; } //TODO
    virtual double deltag() { return 0; } // TODO
    virtual double deltawC(SScNeuron*) { return 0; } //TODO
    virtual double deltagC() { return 0; } // TODO

    virtual double out() { const double c = outC(); return m_act->activate(net())*(1.0-c)+c*highway(); }
    virtual double outC() { return m_cact->activate(netC()); }

    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }
    virtual QList<SScNeuron*> inputsC() const { return m_cin.keys(); }

    virtual bool trainingStep(bool cycleDone) { return false; } //TODO
    virtual double highway() { return m_hw ? m_hw->out() : 0.0; }
    virtual void setHighway(SScNeuron* hw) { m_hw=hw; }

protected:
    SScHighwayNetwork* m_nw;
    int             m_layer, m_nr;
    SScGate         m_in, m_cin;
    SScActivation*  m_act;
    SScActivation*  m_cact;
    SScNeuron*      m_hw;
};
#endif // HWNEURON_HPP
