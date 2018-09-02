#ifndef GATE_HPP
#define GATE_HPP

#include <QHash>

#include "activation/sscactivation.hpp"
#include "activation/sscconnection.hpp"
#include "rnn/sscsignal.hpp"

class SScRNeuron;

class SScGate
{
public:
    explicit SScGate(SScActivation::SSeActivation act = SScActivation::Act_Logistic);
    virtual ~SScGate();
    bool addConnection(SScRNeuron*, double w, SScConnection::SSeConnectionType ctype = SScConnection::Connectiontype_RPROP);
    bool delConnection(SScRNeuron*);
    double get(int t);

private:
    QHash<SScRNeuron*,SScConnection*>   m_con;
    SScActivation*                      m_act;
    double                              m_zeroact;
    SScStdSignal<double>                m_signal;
};

#endif // GATE_HPP
