#ifndef GATE_HPP
#define GATE_HPP

#include <QHash>

#include "activation/sscactivation.hpp"
#include "activation/sscconnection.hpp"
#include "rnn/sscsignal.hpp"

class SScRNeuronBase;

class SScGate
{
public:
    explicit SScGate(SScActivation::SSeActivation act = SScActivation::Act_Logistic);
    virtual ~SScGate();
    bool addConnection(SScRNeuronBase*, double w, SScConnection::SSeConnectionType ctype = SScConnection::Connectiontype_RPROP);
    bool delConnection(SScRNeuronBase*);
    double get(int t);
    void clear() { m_signal.clear(); }

private:
    QHash<SScRNeuronBase*,SScConnection*>   m_con;
    SScActivation*                          m_act;
    double                                  m_zeroact;
    SScStdSignal<double>                    m_signal;
};

#endif // GATE_HPP
