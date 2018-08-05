#ifndef SSNETWORK_HPP
#define SSNETWORK_HPP

#include "sscnetwork_global.h"
#include "sscneuron.hpp"

#include <QMap>
class SSNETWORKSHARED_EXPORT SScNetwork
{
public:
    SScNetwork();
    virtual ~SScNetwork();
    int addNeuron(SScNeuron::SSeNeuronType type);
    bool delNeuron(SScNeuron* n);
    bool delNeuron(int idx);
    int n2idx(SScNeuron* n) const;
    SScNeuron* idx2n(int idx) const;
void connectForward();
    bool contains(SScNeuron* n) const;
    bool connect(SScNeuron* from, SScNeuron* to, double v);
    bool disconnect(SScNeuron* from, SScNeuron* to);
    bool connect(int from, int to, double v);
    bool disconnect(int from, int to);

private:
    bool isFeedForward() const;

    QList<SScNeuron*>  m_neurons;
};

#endif // SSNETWORK_HPP
