#ifndef SSNETWORK_HPP
#define SSNETWORK_HPP

#include "sscnetwork_global.h"
#include "sscneuron.hpp"
#include "../nnhelpers/sscvm.hpp"

#include <QMap>
class SScNetwork
{
public:
    SScNetwork();
    virtual ~SScNetwork();
    inline int addInputNeuron   () { return addNeuron(SScNeuron::NeuronType_Input); }
    inline int addHiddenNeuron  () { return addNeuron(SScNeuron::NeuronType_Hidden); }
    inline int addOutputNeuron  () { return addNeuron(SScNeuron::NeuronType_Output); }
    inline int addBiasNeuron    () { return addNeuron(SScNeuron::NeuronType_Bias); }

    int addNeuron(SScNeuron::SSeNeuronType type, const QString& name = QString());
    bool delNeuron(SScNeuron* n);
    bool delNeuron(int idx);
    int n2idx(SScNeuron* n) const;
    SScNeuron* idx2n(int idx) const;
    void resetDedo() { foreach(SScNeuron* n, m_neurons) n->reset(); }
    void connectForward();
    bool contains(SScNeuron* n) const;
    bool connect(SScNeuron* from, SScNeuron* to, double v);
    bool disconnect(SScNeuron* from, SScNeuron* to);
    bool connect(int from, int to, double v);
    bool disconnect(int from, int to);

    SScVM tpVM() const;
    bool fromVM(const SScVM& vm);

    void trainingStep(bool endOfCycle)
    {
        resetDedo();
        foreach(SScNeuron* n, m_neurons) n->trainingStep();
        if (endOfCycle) foreach(SScNeuron* n, m_neurons) n->endOfCycle();
    }

private:
    bool isFeedForward() const;

    QList<SScNeuron*>  m_neurons;
};

#endif // SSNETWORK_HPP
