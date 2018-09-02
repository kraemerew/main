#ifndef RNEURON_HPP
#define RNEURON_HPP
#include <QMap>
#include <QSharedPointer>
#include <QList>
#include "activation/sscconnection.hpp"
#include "sscsignal.hpp"
#include "gate.hpp"

class SScActivation;

class SScRNeuronBase
{
public:
    virtual double out(int) = 0;
    virtual double err(int) = 0;
    virtual double in (int) = 0;
    virtual double mem(int) = 0;
    virtual void   clear () {};
};

class SScRBiasNeuron : public SScRNeuronBase
{
public:
    explicit SScRBiasNeuron() : SScRNeuronBase() {}
    virtual double out(int) { return 1.0; }
    virtual double err(int) { return 0.0; }
    virtual double in (int) { return 0.0; }
    virtual double mem(int) { return 1.0; }
};

class SScRNeuron : public SScRNeuronBase
{
public:
    explicit SScRNeuron(SScActivation::SSeActivation acttype = SScActivation::Act_Tanh);
    virtual ~SScRNeuron();
    virtual double err(int t);
    virtual double out(int t);
    virtual double in (int t);
    virtual double mem(int t);
    virtual void   clear ();

    void  setInput(SScSignal<double>* s) { m_isig=s; }
    void  setTarget(SScSignal<double>* s) { m_tsig=s; }


    inline SScGate* inputGate  () const { return m_ginput;  }
    inline SScGate* outputGate () const { return m_goutput; }
    inline SScGate* storeGate  () const { return m_gstore;  }
    inline SScGate* memoryGate () const { return m_gmemory; }

protected:
    SScActivation*          m_act;
    double                  m_zeroact;
    SScSignal<double>       *m_isig,   // external input signal / optional
                            *m_tsig;  // external target signal / optional
    SScGate                 *m_ginput,
                            *m_goutput,
                            *m_gmemory,
                            *m_gstore;
    SScStdSignal<double>    m_osig, m_msig;
};

#endif // RNEURON_HPP
